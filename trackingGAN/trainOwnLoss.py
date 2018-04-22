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

import dataPreprocessing
import evaluation
import modelLSTM
import modelLSTMConditional
import modelTrans, modelMLP
import modelTransPath
import modelTransPathConditional
import modelTransPathConditionalBig
import modelTransPathConditionalBigOut
import modelTransPathConditionalBigOutUpsampling
import modelTransPathSmall
import modelVAE
import modelVGG

import os

os.environ["CUDA_VISIBLE_DEVICES"] = "1"

save_rate = 200


def nsLoss(yTrue, yPred):
    return -K.log(yPred)

def wasserstein_loss(y_true, y_pred):
    return K.mean(y_true * y_pred)

def physicsLoss(yTrue, yPred):
    # print("phys: ", yPred.shape)
    # print(yPred)
    errorTrack = evaluation.circleLoss(yPred)
    return errorTrack  # evaluation.computeCircleError(yPred, True)


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


def preTrainConditionalDiscriminatorMomenta(d, dataset, momenta, batch_size, noiseSize, nr_of_steps):
    import dataPreprocessing

    d.trainable = True

    for i in range(nr_of_steps):
        nb_of_real_images = random.randint(1, batch_size)
        realSamples = random.sample(range(dataset.shape[0]), nb_of_real_images)
        random_samples = random.sample(range(dataset.shape[0]), batch_size - nb_of_real_images)
        random_samples_m = random.sample(range(dataset.shape[0]), batch_size - nb_of_real_images)
        random_images = dataset[random_samples,]
        random_momenta = momenta[random_samples_m]
        realImages = dataset[realSamples,]
        real_momenta = momenta[realSamples,]

        x_2 = np.concatenate((real_momenta, random_momenta))
        x = np.concatenate((realImages, random_images))

        y = np.ones([batch_size, 1])
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


def trainTrackConditionalOwnLoss(model=modelTransPathConditionalBigOut, changeSteps=False, batch_size=32, g_mod=1,
                                 d_mod=1, total_steps=40, load=False,
                                 noiseSize=75):
    import dataPreprocessing

    log_path = 'logs'
    callback = TensorBoard(log_path)
    train_names = ['train_loss', 'train_mae']
    val_names = ['val_loss', 'val_mae']

    d, g, AM = model.d_g_AM()

    # plot_model(AM, to_file='model_graps/conditional.png', show_shapes=True)
    if (load):
        d = load_model('models/d_cond.h5')
        g = load_model('models/g_cond.h5')
        AM = load_model('models/AM_cond.h5')

    adam_optimizer_d = optimizers.Adam(lr=0.0004)  # lr=0.0001)  # , decay=4e-8)
    d.compile(loss='binary_crossentropy', optimizer=adam_optimizer_d, metrics=['accuracy'])

    d.trainable = False
    AM.summary()
    # print("False?! ",AM.get_layer("model_1").trainable)
    # print("True? ",AM.get_layer("model_2").trainable)
    # AM.add(d)
    callback.set_model(AM)
    adam_optimizer_g = optimizers.Adam(lr=0.0007)  # , decay=4e-8)
    rms_optimizer_g = optimizers.rmsprop()
    AM.compile(loss={'model_1': 'binary_crossentropy', 'g_out': 'mean_squared_error'}, loss_weights=[.6, .8],
               optimizer=adam_optimizer_g,
               metrics={'model_1': 'accuracy', 'g_out': 'mse'})

    dataset, momenta = dataPreprocessing.loadDataset()

    # d = preTrainConditionalDiscriminator(d, g, dataset, batch_size, noiseSize, 5000)
    genVSdis = 0;

    print(AM.metrics_names)

    for total_step in range(total_steps):
        # print('Training discriminator')
        d.trainable = True
        nb_of_real_images = random.randint(1, batch_size)
        realSamples = random.sample(range(dataset.shape[0]), nb_of_real_images)
        realImages = dataset[realSamples,]

        real_samples_to_compare = random.sample(range(dataset.shape[0]), batch_size)
        real_images_to_compare = dataset[real_samples_to_compare,]
        fake_vetors = dataPreprocessing.getDirection(
            real_images_to_compare)  # np.random.uniform(0, 1, size=[batch_size, 3])
        noise = dataPreprocessing.generateNoise(fake_vetors, noiseSize)
        images_fake = g.predict(noise)

        if total_step % d_mod == 0:
            real_vectors = dataPreprocessing.getDirection(realImages)
            x_2 = np.concatenate((real_vectors, fake_vetors))
            x = np.concatenate((realImages, images_fake))
            y = np.ones([realImages.shape[0] + images_fake.shape[0], 1])
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
            y = np.ones([images_fake.shape[0], 1])
            loss_g = AM.train_on_batch(x=[noise, fake_vetors], y=[y,
                                                                  real_images_to_compare])

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

        if total_step % save_rate == 1:
            increment_statistics()
            dataPreprocessing.saveImages(images_fake, total_step, batch_size)
            dataPreprocessing.saveImages(real_images_to_compare, total_step, batch_size, True)

        if total_step % 1000 == 0:
            d = preTrainConditionalDiscriminator(d, g, dataset, batch_size, noiseSize, 300)

        if total_step % 100 == 1:
            # logs = model.train_on_batch(X_train, Y_train)
            print('Total step nr', total_step)
            write_log(callback, train_names, loss_g, total_step)
            print('D', loss_d)
            print('G', loss_g)
            # print("GvsD", genVSdis, "d_mod", d_mod, "g_mod", g_mod)
    g.save('models/g_cond200.h5', overwrite=True)
    d.save('models/d_cond200.h5', overwrite=True)
    AM.save('models/AM_cond200.h5', overwrite=True)


def trainTrackConditionalOwnLossMomenta(model=modelTransPathConditionalBigOutUpsampling, changeSteps=False,
                                        batch_size=32, g_mod=1,
                                        d_mod=1, total_steps=40, load=False,
                                        noiseSize=75):
    import dataPreprocessing

    log_path = 'logs'
    callback = TensorBoard(log_path)
    train_names = ['train_loss', 'train_mae']
    val_names = ['val_loss', 'val_mae']

    d, g, AM = model.d_g_AM(second_input_size=5)

    # plot_model(AM, to_file='model_graps/conditional.png', show_shapes=True)
    if (load):
        d = load_model('models/d_cond.h5')
        g = load_model('models/g_cond.h5')
        AM = load_model('models/AM_cond.h5')

    adam_optimizer_d = optimizers.Adam(lr=0.0004)  # lr=0.0001)  # , decay=4e-8)
    rms_optimizer_d = optimizers.rmsprop(lr=0.001)
    d.compile(loss='binary_crossentropy', optimizer=rms_optimizer_d, metrics=['accuracy'])

    d.trainable = False
    AM.summary()
    # print("False?! ",AM.get_layer("model_1").trainable)
    # print("True? ",AM.get_layer("model_2").trainable)
    # AM.add(d)
    callback.set_model(AM)
    adam_optimizer_g = optimizers.Adam(lr=0.0007)  # , decay=4e-8)
    rms_optimizer_g = optimizers.rmsprop(lr=0.002)
    AM.compile(loss={'model_1': 'binary_crossentropy', 'g_out': 'mean_squared_error'}, loss_weights=[.6, .8],
               optimizer=rms_optimizer_g,
               metrics={'model_1': 'accuracy', 'g_out': 'mse'})

    dataset, momenta = dataPreprocessing.loadDataset();

    # d = preTrainConditionalDiscriminator(d, g, dataset, batch_size, noiseSize, 5000)
    genVSdis = 0;

    print(AM.metrics_names)

    for total_step in range(total_steps):
        # print('Training discriminator')
        d.trainable = True
        nb_of_real_images = random.randint(1, batch_size)
        realSamples = random.sample(range(dataset.shape[0]), nb_of_real_images)
        realImages = dataset[realSamples,]
        real_momenta = momenta[realSamples,]

        real_samples_to_compare = random.sample(range(dataset.shape[0]), batch_size)
        real_images_to_compare = dataset[real_samples_to_compare,]
        real_momenta_to_compare = momenta[real_samples_to_compare,]
        # fake_vetors = dataPreprocessing.getDirection(
        #    real_images_to_compare)  # np.random.uniform(0, 1, size=[batch_size, 3])
        noise = dataPreprocessing.generateNoise5(real_momenta_to_compare, noiseSize)
        images_fake = g.predict(noise)

        if total_step % d_mod == 0:
            # real_vectors = dataPreprocessing.getDirection(realImages)
            x_2 = np.concatenate((real_momenta, real_momenta_to_compare))
            x = np.concatenate((realImages, images_fake))
            y = np.ones([realImages.shape[0] + images_fake.shape[0], 1])
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
            y = np.ones([images_fake.shape[0], 1])
            loss_g = AM.train_on_batch(x=[noise, real_momenta_to_compare], y=[y,
                                                                              real_images_to_compare])

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

        if total_step % save_rate == 1:
            increment_statistics()
            dataPreprocessing.saveImages(images_fake, total_step, batch_size)
            dataPreprocessing.saveImages(real_images_to_compare, total_step, batch_size, True)

        if total_step % 1000 == 0:
            d = preTrainConditionalDiscriminatorMomenta(d, dataset, momenta, batch_size, noiseSize, 300)

        if total_step % 100 == 1:
            # logs = model.train_on_batch(X_train, Y_train)
            print('Total step nr', total_step)
            write_log(callback, train_names, loss_g, total_step)
            print('D', loss_d)
            print('G', loss_g)
            # print("GvsD", genVSdis, "d_mod", d_mod, "g_mod", g_mod)
    g.save('models/g_cond200.h5', overwrite=True)
    d.save('models/d_cond200.h5', overwrite=True)
    AM.save('models/AM_cond200.h5', overwrite=True)


def plotModel(model):
    plot_model(model, to_file='model_graps/conditional.png', show_shapes=True)


# train(g=modelTrans.generator(), d=modelTrans.discriminator(), d_mod=100, batch_size=32, total_steps=100000)
# trainTrack(g=modelTransPath.generator(), d=modelTransPath.discriminator(), d_mod=10, batch_size=32, total_steps=100000, load=False)
# generator= modelVAE.trainVAEForGAN(5)
# generator.summary()

# trainTrack(g=modelMLP.generator(), d=modelMLP.discriminator(), changeSteps=False,
#            d_mod=3, batch_size=64,
#            total_steps=100000, load=False, noiseSize=100)
# trainTrackConditionalOwnLoss(model=modelTransPathConditionalBigOutUpsampling, changeSteps=False,
#                              d_mod=1, g_mod=5, batch_size=64,
#                              total_steps=100000, load=True, noiseSize=75)


trainTrackConditionalOwnLossMomenta(model=modelTransPathConditionalBigOutUpsampling, changeSteps=False,
                                    d_mod=1, g_mod=4, batch_size=64,
                                    total_steps=100000, load=False, noiseSize=100)
