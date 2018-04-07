import numpy as np
import random
import tensorflow as tf
import os

from keras import Sequential, optimizers, Input, Model
from keras.callbacks import TensorBoard
from keras.layers import Dense, Conv2D, Reshape, LeakyReLU, Conv2DTranspose, Flatten, regularizers
from keras.models import load_model
from keras.optimizers import RMSprop
from keras.utils import to_categorical, plot_model

import keras.backend as K
import modelTrans, modelMLP
import modelTransPath
import modelVGG
import dataPreprocessing

import os
os.environ["CUDA_VISIBLE_DEVICES"]="1"
save_rate = 200

log_path = 'logs'
callback = TensorBoard(log_path)
train_names = ['train_loss', 'train_mae']
val_names = ['val_loss', 'val_mae']


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


def progressiveTrain(_g, _d, dataset, batch_size=32, g_mod=1, d_mod=1, total_steps=50000, save_nr =0):
    g_out = Conv2DTranspose(1, (3, 10), padding='same')(_g.output)
    d_out = Dense(1, activation='sigmoid')(_d.output)
    d = Model(_d.input, d_out)
    g = Model(_g.input, g_out)
    adam_optimizer_d = optimizers.Adam(lr=0.0003)
    rms_optimizer = optimizers.rmsprop(lr=0.00007)
    d.compile(loss='binary_crossentropy', optimizer=adam_optimizer_d, metrics=['accuracy'])

    AM = Sequential()
    AM.add(g)
    d.trainable = False
    AM.add(d)
    callback.set_model(AM)
    rms_optimizerG = optimizers.rmsprop(lr=0.0001)
    adam_optimizer_g = optimizers.Adam(lr=0.0002)
    AM.compile(loss=nsLoss, optimizer=adam_optimizer_g, metrics=['accuracy'])

    # dataset = dataPreprocessing.generateTransTrackTrainSet(True)

    for total_step in range(total_steps):
        # print('Training discriminator')
        d.trainable = True
        noise = np.random.uniform(-1.0, 1.0, size=[batch_size, 100])
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

        if (total_step % save_rate) == 1:
            increment_statistics()
            dataPreprocessing.saveImages(images_fake, total_step, batch_size)

        if (total_step % 100) == 1:
            # logs = model.train_on_batch(X_train, Y_train)
            print('Total step nr', total_step)
            write_log(callback, train_names, loss_g, total_step)
            print('D', loss_d)
            print('G', loss_g)
    g.save('models/g_pro_'+str(save_nr)+'.h5', overwrite=True)
    d.save('models/d_pro_'+str(save_nr)+'.h5', overwrite=True)

    return _g, _d


def trainTrack(batch_size=32, g_mod=1, d_mod=1, total_steps=40, load=False):
    if (load):
        d = load_model('models/d.h5')
        g = load_model('models/g.h5')

    input_size = 100
    image_size = 159 * 3
    image_shape = (159, 3, 1)
    g_x = Input(shape=(input_size,))
    g_f = Dense(100, activation='relu')(g_x)
    g_f2 = Dense(image_size, activation='relu')(g_f)
    g_fR = Reshape((159, 3, 1))(g_f2)

    g_c1 = Conv2DTranspose(40, (20, 3), padding='same', kernel_regularizer=regularizers.l2(0.01))(g_fR)
    g_c1A = LeakyReLU()(g_c1)

    g_c2 = Conv2DTranspose(30, (30, 3), padding='same', kernel_regularizer=regularizers.l2(0.01))(g_c1A)
    g_c2A = LeakyReLU()(g_c2)

    g = Model(g_x, g_c2A)

    d_x = Input(shape=image_shape)
    d_c1 = Conv2D(30, (20, 3), padding='same', kernel_regularizer=regularizers.l2(0.01))(d_x)
    d_c1A = LeakyReLU()(d_c1)

    d_c2 = Conv2D(20, (15, 3), padding='same', kernel_regularizer=regularizers.l2(0.01))(d_c1A)
    d_c2A = LeakyReLU()(d_c2)

    d_c3 = Conv2D(10, (10, 3), padding='same', kernel_regularizer=regularizers.l2(0.01))(d_c2A)
    d_c3A = LeakyReLU()(d_c3)

    d_f = Flatten()(d_c3A)
    d_d1 = Dense(64, activation='relu')(d_f)
    # d_d2 = Dense(64, activation='relu')(d_d1)

    d = Model(d_x, d_d1)

    dataset = dataPreprocessing.generateTransTrackTrainSet(True)

    dataset1 = np.floor(dataset * 100) / 100
    g.summary()
    d.summary()
    g, d = progressiveTrain(g, d, dataset1, d_mod=3, total_steps=100000, save_nr=1)

    g_nc1 = Conv2DTranspose(70, (20, 3), padding='same', kernel_regularizer=regularizers.l2(0.01))(g.output)
    g_nc1A = LeakyReLU()(g_nc1)
    d_n = Dense(64, activation='relu', kernel_regularizer=regularizers.l2(0.01))(d.output)

    g = Model(g.input, g_nc1A)
    d = Model(d.input, d_n)

    dataset2 = np.floor(dataset * 500) / 500
    g, d = progressiveTrain(g, d, dataset2, d_mod=5, save_nr=2)

    dataset3 = np.floor(dataset * 1000) / 1000
    g, d = progressiveTrain(g, d, dataset3, d_mod=4, save_nr=3)

    g_nc2 = Conv2DTranspose(40, (20, 3), padding='same', kernel_regularizer=regularizers.l2(0.01))(g.output)
    g_nc2A = LeakyReLU()(g_nc2)
    d_n2 = Dense(32, activation='relu', kernel_regularizer=regularizers.l2(0.01))(d.output)

    g = Model(g.input, g_nc2A)
    d = Model(d.input, d_n2)

    dataset4 = np.floor(dataset * 1500) / 1500
    g, d = progressiveTrain(g, d, dataset4, d_mod=5, save_nr=4)

    g_nc3 = Conv2DTranspose(20, (20, 3), padding='same', kernel_regularizer=regularizers.l2(0.01))(g.output)
    g_nc3A = LeakyReLU()(g_nc3)
    d_n3 = Dense(16, activation='relu', kernel_regularizer=regularizers.l2(0.01))(d.output)

    g = Model(g.input, g_nc3A)
    d = Model(d.input, d_n3)

    g, d = progressiveTrain(g, d, dataset, d_mod=4, total_steps=100000, save_nr=5)




# train(g=modelTrans.generator(), d=modelTrans.discriminator(), d_mod=100, batch_size=32, total_steps=100000)

trainTrack()
# trainTrack(g=modelVGG.generator(), d=modelVGG.discriminator(), d_mod=10, batch_size=32, total_steps=100000, load=False)
