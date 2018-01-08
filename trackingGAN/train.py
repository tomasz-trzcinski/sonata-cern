import numpy as np
import random
import os

from keras import Sequential, optimizers
from keras.callbacks import TensorBoard
from keras.optimizers import RMSprop
from keras.utils import to_categorical

import keras.backend as K
import modelTrans, modelMLP

tensorboard = TensorBoard(log_dir='logs', histogram_freq=0, write_graph=True, write_images=True)

def nsLoss(yTrue, yPred):
    return -K.log(yPred)


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


def train(g, d, batch_size=32, epochs_generator=402, epochs_discriminator=102, total_steps=40):
    import dataPreprocessing

    rms_optimizer = optimizers.rmsprop(lr=0.00007, decay=3e-7)
    d.compile(loss='binary_crossentropy', optimizer=rms_optimizer, metrics=['accuracy'])

    AM = Sequential()
    AM.add(g)
    d.trainable = False
    AM.add(d)
    rms_optimizerG = optimizers.rmsprop(lr=0.00005, decay=3e-7)
    # adam_optimizer = optimizers.Adam(lr=0.0001)
    AM.compile(loss=nsLoss, optimizer=rms_optimizerG, metrics=['accuracy'])

    dataset = dataPreprocessing.generateTransTrainSet()

    for total_step in range(total_steps):
        print('Total step nr', total_step)
        print('Training discriminator')
        d.trainable = True
        for i in range(epochs_discriminator):
            noise = np.random.uniform(-1.0, 1.0, size=[batch_size, 100])
            images_fake = g.predict(noise)
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

            loss = d.train_on_batch(x=newX, y=newY)
            if (i % 100) == 1:
                print(i, loss)

        d.trainable = False

        print('Training generator')
        for i in range(epochs_generator):
            noise = np.random.uniform(-1.0, 1.0, size=[batch_size, 100])
            images_fake = g.predict(noise)
            y = np.ones([batch_size, 1])
            loss = AM.train_on_batch(noise, y)
            if (i % 100) == 1:
                print(i, loss)
                dataPreprocessing.saveImages(images_fake, total_step, i)


train(g=modelTrans.generator(), d=modelTrans.discriminator(), batch_size=32, epochs_generator=1002, epochs_discriminator=102,
      total_steps=30)

