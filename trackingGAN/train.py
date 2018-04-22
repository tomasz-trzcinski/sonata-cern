import numpy as np
import random
import tensorflow as tf
import os

from keras import Sequential, optimizers, Model, Input
from keras.callbacks import TensorBoard
from keras.layers import Conv2DTranspose, Dense
from keras.models import load_model
from keras.optimizers import RMSprop
from keras.utils import to_categorical, plot_model

import keras.backend as K

import modelLSTM
import modelLSTMConditional
import modelTrans, modelMLP
import modelTransPath
import modelTransPathConditional
import modelTransPathConditionalBig
import modelTransPathSmall
import modelVAE
import modelVGG

import os

import modelWDCGAN
import modelWDCGANBig
import modelWDCGANConditionalBig
import modelWDCGANupsampling

os.environ["CUDA_VISIBLE_DEVICES"] = "1"

save_rate = 200


def nsLoss(yTrue, yPred):
    return -K.log(yPred)


def increment_statistics():
    f = open('statistics', 'r+')
    x = int(f.read())
    y = x + save_rate
    f.seek(0)
    f.truncate()
    f.write(str(y))
    f.close()


def write_log(callback, names, logs, batch_no):
    for name, value in zip(names, logs):
        summary = tf.Summary()
        summary_value = summary.value.add()
        summary_value.simple_value = value
        summary_value.tag = name
        callback.writer.add_summary(summary, batch_no)
        callback.writer.flush()


def trainFristModel():
    import firstModel
    from dataPreprocessing import generateTrainSet
    trainX = generateTrainSet();
    trainX = trainX.reshape(trainX.shape[0], 500, 500, 500, 1)
    print(trainX.shape)
    labels = np.array((1, 1, 1))
    # labels = labels.astype(float)
    labels = to_categorical(labels.reshape(3, 1))
    # mat = convertToMatrix('data/results_clusters.txt')
    # plotMatrix(mat)
    print('Starts with the Model')
    D = firstModel.discriminator()
    D.compile(loss='categorical_crossentropy', optimizer='adam')
    h = D.fit(trainX, labels, 1, 10)
    print(h)


def trainDiscriminator():
    import modelTrans
    from dataPreprocessing import generateTransTrainSet
    labels = np.array((1, 1, 1))
    # labels = to_categorical(labels.reshape(3,1))
    trainSet = generateTransTrainSet()
    # trainSet = trainSet.reshape(trainSet.shape[0], 159, 5, 1)
    # print(trainSet.shape)
    D = modelTrans.discriminator()
    rms_optimizer = optimizers.rmsprop(lr=0.00007, decay=3e-7)
    D.compile(loss='binary_crossentropy', optimizer=rms_optimizer, metrics=['accuracy'])
    # D.fit(x=trainSet, y=labels, batch_size=1, epochs=100)
    return D


def wasserstein_loss(y_true, y_pred):
    return K.mean(y_true * y_pred)


def train_wasserstein(g, d, changeSteps=False, batch_size=32, g_mod=1, d_mod=1, total_steps=40, load=False,
                      noiseSize=100,
                      noiseDist='uniform'):
    import dataPreprocessing

    log_path = 'logs'
    callback = TensorBoard(log_path)
    train_names = ['train_loss', 'train_mae']
    val_names = ['val_loss', 'val_mae']

    if (load):
        d = load_model('models/d_small.h5')
        g = load_model('models/g_small.h5')

    adam_optimizer_d = optimizers.Adam(lr=0.0001)  # , decay=4e-8)
    rms_optimizer = optimizers.rmsprop(lr=0.0005)
    d.compile(loss=wasserstein_loss, optimizer=adam_optimizer_d, metrics=['accuracy'])

    AM = Sequential()
    AM.add(g)
    d.trainable = False
    AM.add(d)
    callback.set_model(AM)
    rms_optimizerG = optimizers.rmsprop(lr=0.0005)
    adam_optimizer_g = optimizers.Adam(lr=0.0002)  # , decay=4e-8)
    AM.compile(loss=wasserstein_loss, optimizer=adam_optimizer_g, metrics=['accuracy'])

    dataset = dataPreprocessing.generateTransTrackTrainSet(True,1500)

    genVSdis = 0;

    for total_step in range(total_steps):
        # print('Training discriminator')
        d.trainable = True
        if (noiseDist == 'normal'):
            noise = np.random.normal(0, 1, size=[batch_size, noiseSize])
        else:
            noise = np.random.uniform(0, 1, size=[batch_size, noiseSize])
        images_fake = g.predict(noise)

        if total_step % d_mod == 0:
            nb_of_real_images = random.randint(1, batch_size)
            realSamples = random.sample(range(dataset.shape[0]), nb_of_real_images)
            realImages = dataset[realSamples,]
            x = np.concatenate((realImages, images_fake))
            y = np.ones([realImages.shape[0] + batch_size, 1])
            y[realImages.shape[0]:, :] = 0

            shuffled = list([j for j in range(x.__len__())])
            newX = x
            newY = y
            random.shuffle(shuffled)
            for index, value in enumerate(shuffled):
                newY[index] = y[value]
                newX[index] = x[value]

            for l in d.layers:
                weights = l.get_weights()
                weights = [np.clip(w, -0.0008, 0.0008) for w in weights]
                l.set_weights(weights)

            loss_d = d.train_on_batch(x=newX, y=newY)

        d.trainable = False

        # print('Training generator')
        if total_step % g_mod == 0:
            y = np.ones([batch_size, 1])
            loss_g = AM.train_on_batch(noise, y)

        if changeSteps:
            if loss_d[1] < 0.5:
                genVSdis = min(genVSdis + 1, 200)
            else:
                genVSdis = max(genVSdis - 1, -200)

            if genVSdis > 100:
                d_mod = max(1, d_mod - 1)
            if genVSdis < -100:
                d_mod = min(d_mod + 1, 100)

        if (total_step % save_rate) == 1:
            increment_statistics()
            dataPreprocessing.saveImages(images_fake, total_step, batch_size)

        if (total_step % 100) == 1:
            # logs = model.train_on_batch(X_train, Y_train)
            print('Total step nr', total_step)
            write_log(callback, train_names, loss_g, total_step)
            print('D', loss_d)
            print('G', loss_g)
            print("GvsD", genVSdis, "d_mod", d_mod, "g_mod", g_mod)
    g.save('models/g_wgan.h5', overwrite=True)
    d.save('models/d_wgan.h5', overwrite=True)


def preTrainConditionalDiscriminator(d, g, dataset, batch_size, noiseSize, nr_of_steps):
    import dataPreprocessing

    d.trainable = True
    for i in range(nr_of_steps):
        random_vetors = np.random.uniform(0, 1, size=[batch_size, 3])
        # noise = dataPreprocessing.generateNoise(random_vetors, noiseSize)
        # images_fake = g.predict(noise)
        random_samples = random.sample(range(dataset.shape[0]), batch_size)
        random_images = dataset[random_samples,]

        nb_of_real_images = random.randint(1, batch_size)
        realSamples = random.sample(range(dataset.shape[0]), nb_of_real_images)
        realImages = dataset[realSamples,]
        real_vectors = dataPreprocessing.getDirection(realImages)
        x_2 = np.concatenate((real_vectors, random_vetors))
        x = np.concatenate((realImages, random_images))
        y = np.ones([realImages.shape[0] + batch_size, 1])
        y[realImages.shape[0]:, :] = 0

        shuffled = list([j for j in range(x.__len__())])
        newX = x
        newX2 = x_2
        newY = y
        random.shuffle(shuffled)
        for index, value in enumerate(shuffled):
            newY[index] = y[value]
            newX[index] = x[value]
            newX2[index] = x_2[value]
        # print(newX.shape[0])
        loss_d = d.train_on_batch(x=[newX, x_2], y=newY)  # np.ones(shape=(newX.shape[0], 3))
        if i % 100 == 1:
            print(i, loss_d)
    return d


def train_wasserstein_conditional(model=modelWDCGANConditionalBig, changeSteps=False, batch_size=32, g_mod=1, d_mod=1,
                                  total_steps=40, load=False,
                                  noiseSize=75):
    import dataPreprocessing

    log_path = 'logs'
    callback = TensorBoard(log_path)
    train_names = ['train_loss', 'train_mae']
    val_names = ['val_loss', 'val_mae']

    d, g, AM = model.d_g_AM()

    # plot_model(AM, to_file='model_graps/conditional.png', show_shapes=True)
    if (load):
        d = load_model('models/d_conv.h5')
        g = load_model('models/g_conv.h5')

    rms_optimizer = optimizers.rmsprop(lr=0.0005)
    adam_optimizer_d = optimizers.Adam(lr=0.001)  # lr=0.0001)  # , decay=4e-8)
    d.compile(loss=wasserstein_loss, optimizer=rms_optimizer, metrics=['accuracy'])

    d.trainable = False
    AM.summary()
    # print("False?! ",AM.get_layer("model_1").trainable)
    # print("True? ",AM.get_layer("model_2").trainable)
    # AM.add(d)
    callback.set_model(AM)
    adam_optimizer_g = optimizers.Adam(lr=0.001)  # , decay=4e-8)
    rms_optimizer_g = optimizers.rmsprop(lr=0.0005)
    AM.compile(loss=wasserstein_loss, optimizer=rms_optimizer_g, metrics=['accuracy'])

    dataset = dataPreprocessing.generateTransTrackTrainSet(scale=True, nrOfLevels=1000)

    # d = preTrainConditionalDiscriminator(d, g, dataset, batch_size, noiseSize, 5000)
    genVSdis = 0;

    for total_step in range(total_steps):
        # print('Training discriminator')
        d.trainable = True
        random_vetors = np.random.uniform(0, 1, size=[batch_size, 3])
        noise = dataPreprocessing.generateNoise(random_vetors, noiseSize)
        images_fake = g.predict(noise)

        if total_step % d_mod == 0:
            nb_of_real_images = random.randint(1, batch_size)
            realSamples = random.sample(range(dataset.shape[0]), nb_of_real_images)
            realImages = dataset[realSamples,]
            real_vectors = dataPreprocessing.getDirection(realImages)
            x_2 = np.concatenate((real_vectors, random_vetors))
            x = np.concatenate((realImages, images_fake))
            y = np.ones([realImages.shape[0] + batch_size, 1])
            y[realImages.shape[0]:, :] = 0

            shuffled = list([j for j in range(x.__len__())])
            newX = x
            newX2 = x_2
            newY = y
            random.shuffle(shuffled)
            for index, value in enumerate(shuffled):
                newY[index] = y[value]
                newX[index] = x[value]
                newX2[index] = x_2[value]
            # print(newX.shape[0])

            for l in d.layers:
                weights = l.get_weights()
                weights = [np.clip(w, -0.01, 0.01) for w in weights]
                l.set_weights(weights)

            loss_d = d.train_on_batch(x=[newX, x_2], y=newY)  # np.ones(shape=(newX.shape[0], 3))

        d.trainable = False
        # print("True? ", AM.get_layer("model_2").trainable)
        # print('Training generator')

        if total_step % g_mod == 0:
            y = np.ones([batch_size, 1])
            loss_g = AM.train_on_batch(x=[noise, random_vetors], y=y)  # np.ones(shape=(noise.shape[0], 3))]

        # if total_step % 25000 ==0:
        #     d = preTrainConditionalDiscriminator(d,g,dataset,32,75,1000)
        if changeSteps:
            if loss_d[1] < 0.5:
                genVSdis = min(genVSdis + 1, 200)
            else:
                genVSdis = max(genVSdis - 1, -200)

            if genVSdis > 100:
                d_mod = max(1, d_mod - 1)
            if genVSdis < -100:
                d_mod = min(d_mod + 1, 100)

        if (total_step % save_rate) == 1:
            increment_statistics()
            dataPreprocessing.saveImages(images_fake, total_step, batch_size)

        if (total_step % 100) == 1:
            # logs = model.train_on_batch(X_train, Y_train)
            print('Total step nr', total_step)
            write_log(callback, train_names, loss_g, total_step)
            print('D', loss_d)
            print('G', loss_g)
            # print("GvsD", genVSdis, "d_mod", d_mod, "g_mod", g_mod)
    g.save('models/g_cond.h5', overwrite=True)
    d.save('models/d_cond.h5', overwrite=True)
    AM.save('models/AM_cond.h5', overwrite=True)


def trainTrackConditional(model=modelTransPathConditional, changeSteps=False, batch_size=32, g_mod=1, d_mod=1,
                          total_steps=40, load=False,
                          noiseSize=75):
    import dataPreprocessing

    log_path = 'logs'
    callback = TensorBoard(log_path)
    train_names = ['train_loss', 'train_mae']
    val_names = ['val_loss', 'val_mae']

    d, g, AM = model.d_g_AM()

    # plot_model(AM, to_file='model_graps/conditional.png', show_shapes=True)
    if (load):
        d = load_model('models/d_conv.h5')
        g = load_model('models/g_conv.h5')

    adam_optimizer_d = optimizers.Adam(lr=0.001)  # lr=0.0001)  # , decay=4e-8)
    d.compile(loss='binary_crossentropy', optimizer=adam_optimizer_d, metrics=['accuracy'])

    d.trainable = False
    AM.summary()
    # print("False?! ",AM.get_layer("model_1").trainable)
    # print("True? ",AM.get_layer("model_2").trainable)
    # AM.add(d)
    callback.set_model(AM)
    adam_optimizer_g = optimizers.Adam(lr=0.001)  # , decay=4e-8)
    rms_optimizer_g = optimizers.rmsprop()
    AM.compile(loss='binary_crossentropy', optimizer=adam_optimizer_g, metrics=['accuracy'])

    dataset = dataPreprocessing.generateTransTrackTrainSet(scale=True, nrOfLevels=1000)

    # d = preTrainConditionalDiscriminator(d, g, dataset, batch_size, noiseSize, 5000)
    genVSdis = 0;

    for total_step in range(total_steps):
        # print('Training discriminator')
        d.trainable = True
        random_vetors = np.random.uniform(0, 1, size=[batch_size, 3])
        noise = dataPreprocessing.generateNoise(random_vetors, noiseSize)
        images_fake = g.predict(noise)

        if total_step % d_mod == 0:
            nb_of_real_images = random.randint(1, batch_size)
            realSamples = random.sample(range(dataset.shape[0]), nb_of_real_images)
            realImages = dataset[realSamples,]
            real_vectors = dataPreprocessing.getDirection(realImages)
            x_2 = np.concatenate((real_vectors, random_vetors))
            x = np.concatenate((realImages, images_fake))
            y = np.ones([realImages.shape[0] + batch_size, 1])
            y[realImages.shape[0]:, :] = 0

            shuffled = list([j for j in range(x.__len__())])
            newX = x
            newX2 = x_2
            newY = y
            random.shuffle(shuffled)
            for index, value in enumerate(shuffled):
                newY[index] = y[value]
                newX[index] = x[value]
                newX2[index] = x_2[value]
            # print(newX.shape[0])
            loss_d = d.train_on_batch(x=[newX, x_2], y=newY)  # np.ones(shape=(newX.shape[0], 3))

        d.trainable = False
        # print("True? ", AM.get_layer("model_2").trainable)
        # print('Training generator')

        if total_step % g_mod == 0:
            y = np.ones([batch_size, 1])
            loss_g = AM.train_on_batch(x=[noise, random_vetors], y=y)  # np.ones(shape=(noise.shape[0], 3))]

        # if total_step % 25000 ==0:
        #     d = preTrainConditionalDiscriminator(d,g,dataset,32,75,1000)
        if changeSteps:
            if loss_d[1] < 0.5:
                genVSdis = min(genVSdis + 1, 200)
            else:
                genVSdis = max(genVSdis - 1, -200)

            if genVSdis > 100:
                d_mod = max(1, d_mod - 1)
            if genVSdis < -100:
                d_mod = min(d_mod + 1, 100)

        if (total_step % save_rate) == 1:
            increment_statistics()
            dataPreprocessing.saveImages(images_fake, total_step, batch_size)

        if (total_step % 100) == 1:
            # logs = model.train_on_batch(X_train, Y_train)
            print('Total step nr', total_step)
            write_log(callback, train_names, loss_g, total_step)
            print('D', loss_d)
            print('G', loss_g)
            # print("GvsD", genVSdis, "d_mod", d_mod, "g_mod", g_mod)
    g.save('models/g_cond.h5', overwrite=True)
    d.save('models/d_cond.h5', overwrite=True)
    AM.save('models/AM_cond.h5', overwrite=True)


def trainTrack(g, d, changeSteps=False, batch_size=32, g_mod=1, d_mod=1, total_steps=40, load=False, noiseSize=100,
               noiseDist='uniform'):
    import dataPreprocessing

    log_path = 'logs'
    callback = TensorBoard(log_path)
    train_names = ['train_loss', 'train_mae']
    val_names = ['val_loss', 'val_mae']

    if (load):
        d = load_model('models/d_small.h5')
        g = load_model('models/g_small.h5')

    # g_out = Conv2DTranspose(1, (3, 10), padding='same')(g.output)
    # d_out = Dense(1, activation='sigmoid')(d.output)
    # d = Model(d.input, d_out)
    # g = Model(g.input, g_out)

    adam_optimizer_d = optimizers.Adam(lr=0.0001)  # , decay=4e-8)
    rms_optimizer = optimizers.rmsprop(lr=0.00007)
    d.compile(loss='binary_crossentropy', optimizer=adam_optimizer_d, metrics=['accuracy'])

    AM = Sequential()
    AM.add(g)
    d.trainable = False
    AM.add(d)
    callback.set_model(AM)
    rms_optimizerG = optimizers.rmsprop(lr=0.00002)
    adam_optimizer_g = optimizers.Adam(lr=0.0002)  # , decay=4e-8)
    AM.compile(loss=nsLoss, optimizer=adam_optimizer_g, metrics=['accuracy'])

    dataset = dataPreprocessing.generateTransTrackTrainSet(True)

    genVSdis = 0;

    for total_step in range(total_steps):
        # print('Training discriminator')
        d.trainable = True
        if (noiseDist == 'normal'):
            noise = np.random.normal(0, 1, size=[batch_size, noiseSize])
        else:
            noise = np.random.uniform(0, 1, size=[batch_size, noiseSize])
        images_fake = g.predict(noise)

        if total_step % d_mod == 0:
            nb_of_real_images = random.randint(1, batch_size)
            realSamples = random.sample(range(dataset.shape[0]), nb_of_real_images)
            realImages = dataset[realSamples,]
            x = np.concatenate((realImages, images_fake))
            y = np.ones([realImages.shape[0] + batch_size, 1])
            y[realImages.shape[0]:, :] = 0

            shuffled = list([j for j in range(x.__len__())])
            newX = x
            newY = y
            random.shuffle(shuffled)
            for index, value in enumerate(shuffled):
                newY[index] = y[value]
                newX[index] = x[value]

            loss_d = d.train_on_batch(x=newX, y=newY)

        d.trainable = False

        # print('Training generator')
        if total_step % g_mod == 0:
            y = np.ones([batch_size, 1])
            loss_g = AM.train_on_batch(noise, y)

        if changeSteps:
            if loss_d[1] < 0.5:
                genVSdis = min(genVSdis + 1, 200)
            else:
                genVSdis = max(genVSdis - 1, -200)

            if genVSdis > 100:
                d_mod = max(1, d_mod - 1)
            if genVSdis < -100:
                d_mod = min(d_mod + 1, 100)

        if (total_step % save_rate) == 1:
            increment_statistics()
            dataPreprocessing.saveImages(images_fake, total_step, batch_size)

        if (total_step % 100) == 1:
            # logs = model.train_on_batch(X_train, Y_train)
            print('Total step nr', total_step)
            write_log(callback, train_names, loss_g, total_step)
            print('D', loss_d)
            print('G', loss_g)
            print("GvsD", genVSdis, "d_mod", d_mod, "g_mod", g_mod)
    g.save('models/g_LSTM.h5', overwrite=True)
    d.save('models/d_LSTM.h5', overwrite=True)


def plotModel(model):
    plot_model(model, to_file='model_graps/conditional.png', show_shapes=True)


# train(g=modelTrans.generator(), d=modelTrans.discriminator(), d_mod=100, batch_size=32, total_steps=100000)
# trainTrack(g=modelTransPath.generator(), d=modelTransPath.discriminator(), d_mod=10, batch_size=32, total_steps=100000, load=False)
# generator= modelVAE.trainVAEForGAN(5)
# generator.summary()

# trainTrack(g=modelTransPathSmall.generator(), d=modelTransPathSmall.discriminator(), changeSteps=False,
#            d_mod=1,g_mod=5, batch_size=32,
#            total_steps=100000, load=False, noiseSize=100)

# trainTrackConditional(model=modelTransPathConditional, changeStepsin=False,
#                       d_mod=1, g_mod=10, batch_size=64,
#                       total_steps=100000, load=False, noiseSize=75)

train_wasserstein(g=modelWDCGANupsampling.generator(), d=modelWDCGANupsampling.discriminator(), changeSteps=False,
                  d_mod=1, g_mod=5, batch_size=64,
                  total_steps=100000, load=False, noiseSize=99)


# train_wasserstein_conditional(changeSteps=False,
#                               d_mod=1, g_mod=5, batch_size=64,
#                               total_steps=400000, load=False, noiseSize=75)
