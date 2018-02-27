import numpy as np
import random
import tensorflow as tf
import os

from keras import Sequential, optimizers, Model
from keras.callbacks import TensorBoard
from keras.layers import Conv2DTranspose, Dense
from keras.models import load_model
from keras.optimizers import RMSprop
from keras.utils import to_categorical, plot_model

import keras.backend as K

import modelLSTM
import modelTrans, modelMLP
import modelTransPath
import modelTransPathSmall
import modelVAE
import modelVGG

import os

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


# def trainDCGAN():
#     batch_size = 32
#     train_steps = 402
#     train_steps_discriminator = 102
#     total_steps = 40
#     import modelTrans, dataPreprocessing
#     import dataPreprocessing
#     rms_optimizer = optimizers.rmsprop(lr=0.00005, decay=3e-7)
#
#     g = modelTrans.generator()
#     d = trainDiscriminator()
#     # g.compile(loss='binary_crossentropy', optimizer='adam')
#     # d = trainDiscriminator()
#
#     AM = Sequential()
#     AM.add(g)
#     d.trainable = False
#     AM.add(d)
#     adam_optimizer = optimizers.Adam(lr=0.0001)
#     AM.compile(loss=nsLoss, optimizer=rms_optimizer, metrics=['accuracy'])
#
#     realImages = dataPreprocessing.generateTransTrainSet()
#
#     for total_step in range(total_steps):
#         print('Total step nr', total_step)
#         print('Training discriminator')
#         d.trainable = True
#         # d.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])
#         for i in range(train_steps_discriminator):
#             noise = np.random.uniform(-1.0, 1.0, size=[batch_size, 100])
#             images_fake = g.predict(noise)
#             x = np.concatenate((realImages, images_fake))
#             y = np.ones([realImages.shape[0] + batch_size, 1])
#             y[realImages.shape[0]:, :] = 0
#
#             shuffled = list([j for j in range(x.__len__())])
#             newX = x
#             newY = y
#             random.shuffle(shuffled)
#             for index, value in enumerate(shuffled):
#                 # print(index, value)
#                 newY[index] = y[value]
#                 newX[index] = x[value]
#
#             loss = d.train_on_batch(x=newX, y=newY)
#             if (i % 100) == 1:
#                 print(i, loss)
#
#         # optimizer = RMSprop(lr=0.0001, decay=3e-8)
#         d.trainable = False
#
#         print('Training generator')
#         for i in range(train_steps):
#             noise = np.random.uniform(-1.0, 1.0, size=[batch_size, 100])
#             images_fake = g.predict(noise)
#             # x = np.concatenate((realImages, images_fake))
#             # y = np.ones([realImages.shape[0] + batch_size, 1])
#             # y[realImages.shape[0]:, :] = 0
#             # d.train_on_batch(x, y)
#             y = np.ones([batch_size, 1])
#             loss = AM.train_on_batch(noise, y)
#             if (i % 100) == 1:
#                 print(i, loss)
#                 dataPreprocessing.saveImages(images_fake, i)
#                 # print(images_fake)
#         # print(d.predict(images_fake))
#         # d = modelTrans.discriminator()
#
#
# def trainMLP():
#     batch_size = 32
#     train_steps = 402
#     train_steps_discriminator = 102
#     total_steps = 40
#     import modelMLP, modelTrans
#     import dataPreprocessing
#
#     g = modelMLP.generator()
#     d = modelTrans.discriminator()
#     rms_optimizer = optimizers.rmsprop(lr=0.00007, decay=3e-7)
#     d.compile(loss='binary_crossentropy', optimizer=rms_optimizer, metrics=['accuracy'])
#
#     AM = Sequential()
#     AM.add(g)
#     d.trainable = False
#     AM.add(d)
#     rms_optimizerG = optimizers.rmsprop(lr=0.00005, decay=3e-7)
#     # adam_optimizer = optimizers.Adam(lr=0.0001)
#     AM.compile(loss=nsLoss, optimizer=rms_optimizerG, metrics=['accuracy'])
#
#     dataset = dataPreprocessing.generateTransTrainSet()
#
#     for total_step in range(total_steps):
#         print('Total step nr', total_step)
#         print('Training discriminator')
#         d.trainable = True
#         for i in range(train_steps_discriminator):
#             noise = np.random.uniform(-1.0, 1.0, size=[batch_size, 100])
#             images_fake = g.predict(noise)
#             nb_of_real_images = random.randint(1, batch_size)
#             realSamples = random.sample(range(dataset.shape[0]), nb_of_real_images)
#             realImages = dataset[realSamples,]
#             x = np.concatenate((realImages, images_fake))
#             y = np.ones([realImages.shape[0] + batch_size, 1])
#             y[realImages.shape[0]:, :] = 0
#
#             shuffled = list([j for j in range(x.__len__())])
#             newX = x
#             newY = y
#             random.shuffle(shuffled)
#             for index, value in enumerate(shuffled):
#                 newY[index] = y[value]
#                 newX[index] = x[value]
#
#             loss = d.train_on_batch(x=newX, y=newY)
#             if (i % 100) == 1:
#                 print(i, loss)
#
#         d.trainable = False
#
#         print('Training generator')
#         for i in range(train_steps):
#             noise = np.random.uniform(-1.0, 1.0, size=[batch_size, 100])
#             images_fake = g.predict(noise)
#             y = np.ones([batch_size, 1])
#             loss = AM.train_on_batch(noise, y)
#             if (i % 100) == 1:
#                 print(i, loss)
#                 dataPreprocessing.saveImages(images_fake, total_step, i)


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

    g_out = Conv2DTranspose(1, (3, 10), padding='same')(g.output)
    d_out = Dense(1, activation='sigmoid')(d.output)
    d = Model(d.input, d_out)
    g = Model(g.input, g_out)

    adam_optimizer_d = optimizers.Adam(lr=0.0001)#, decay=4e-8)
    rms_optimizer = optimizers.rmsprop(lr=0.00007)
    d.compile(loss='binary_crossentropy', optimizer=adam_optimizer_d, metrics=['accuracy'])

    AM = Sequential()
    AM.add(g)
    d.trainable = False
    AM.add(d)
    callback.set_model(AM)
    rms_optimizerG = optimizers.rmsprop(lr=0.00002)
    adam_optimizer_g = optimizers.Adam(lr=0.0002)#, decay=4e-8)
    AM.compile(loss='binary_crossentropy', optimizer=adam_optimizer_g, metrics=['accuracy'])

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
    g.save('models/g_small.h5', overwrite=True)
    d.save('models/d_small.h5', overwrite=True)


def plotModel():
    g = modelMLP.generator()
    d = modelMLP.discriminator()
    AM = Sequential()
    AM.add(g)
    AM.add(d)
    AM.summary()
    plot_model(g, to_file='model_graps/MLPgenerator.png', show_shapes=True)


# train(g=modelTrans.generator(), d=modelTrans.discriminator(), d_mod=100, batch_size=32, total_steps=100000)
# trainTrack(g=modelTransPath.generator(), d=modelTransPath.discriminator(), d_mod=10, batch_size=32, total_steps=100000, load=False)
# generator= modelVAE.trainVAEForGAN(5)
# generator.summary()
trainTrack(g=modelTransPathSmall.generator(), d=modelTransPathSmall.discriminator(), changeSteps=False, d_mod=3, batch_size=128,
           total_steps=200000, load=False, noiseDist='uniform')
