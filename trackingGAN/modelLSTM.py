import keras.backend as K
from keras.layers import (Input, Dense, Reshape, Flatten, Lambda, merge,
                          Dropout, BatchNormalization, Activation, Embedding, LSTM, TimeDistributed)
from keras.layers.advanced_activations import LeakyReLU
from keras.layers.convolutional import (UpSampling2D, Conv2D, ZeroPadding2D, Conv2DTranspose,
                                        AveragePooling2D)

from keras.models import Model, Sequential


def discriminator():
    D = Sequential()
    dropoutratio = 0.3
    image = (159, 3, 1)

    D.add(Reshape((3, 159), input_shape=image))
    D.add(LSTM(159))

    D.add(Dense(64))
    D.add(Activation('relu'))
    D.add(Dense(1))
    D.add(Activation('sigmoid'))
    D.summary()
    return D


def generator():
    G = Sequential()
    inputDim = 100
    depth = 159 * 3
    # netDepth=8
    G.add(Dense(depth, input_shape=(inputDim,)))
    G.add(Activation('relu'))
    G.add(Dense(depth))
    G.add(Reshape((3, 159)))
    G.add(LSTM(159, return_sequences=True))
    G.add(Reshape((159, 3, 1)))
    G.summary()
    return G
