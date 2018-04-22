import keras.backend as K
from keras.layers import (Input, Dense, Reshape, Flatten, Lambda, merge,
                          Dropout, BatchNormalization, Activation, Embedding)
from keras.layers.advanced_activations import LeakyReLU
from keras.layers.convolutional import (UpSampling2D, Conv2D, ZeroPadding2D, Conv2DTranspose,
                                        AveragePooling2D)

from keras.models import Model, Sequential


def discriminator():
    D = Sequential()
    dropoutratio = 0.3
    image = (159, 3, 1)

    D.add(Conv2D(64, (20, 3), border_mode='same', input_shape=image))
    D.add(LeakyReLU())
    D.add(Dropout(0.2))

    D.add(ZeroPadding2D((1, 1)))
    D.add(Conv2D(32, (15, 3), border_mode='valid'))
    D.add(LeakyReLU(alpha=0.2))
    D.add(BatchNormalization())
    D.add(Dropout(dropoutratio))

    D.add(ZeroPadding2D((1, 1)))
    D.add(Conv2D(32, (20, 3), border_mode='valid'))
    D.add(LeakyReLU(alpha=0.2))
    D.add(BatchNormalization())
    D.add(Dropout(dropoutratio))
    #
    #
    # D.add(AveragePooling2D(pool_size=(2, 2)))
    # D.add(Dense(1,activation='softmax'))
    # D.add(Flatten())
    # D.add(Dense(64,activation='relu'))
    D.add(Flatten())
    D.add(Dense(128))
    D.add(Activation('relu'))
    D.add(Dense(64))
    D.add(Activation('relu'))
    D.add(Dense(1,activation='linear'))
    #D.add(Activation('sigmoid'))
    D.summary()
    return D


def generator():
    G = Sequential()
    inputDim = 100
    depth = 159 * 3
    # netDepth=8
    G.add(Dense(depth, input_shape=(inputDim,)))
    G.add(Dense(depth, input_shape=(inputDim,)))
    G.add(Activation('relu'))
    G.add(Reshape((159, 3, 1)))

    G.add(Conv2DTranspose(depth // 2, (10, 3), padding='same'))
    G.add(BatchNormalization())
    G.add(Activation('relu'))
    # G.add(UpSampling2D())

    G.add(Conv2DTranspose(depth // 4, (15, 3), padding='same'))
    G.add(BatchNormalization(momentum=0.8))
    G.add(Activation('relu'))
    #G.add(Dropout(0.4))
    # G.add(UpSampling2D())

    G.add(Conv2DTranspose(int(depth // 8), (20, 3), padding='same'))
    G.add(BatchNormalization(momentum=0.8))
    G.add(Activation('relu'))
    #G.add(Dropout(0.4))

    G.add(Conv2DTranspose(1, (1, 20), padding='same'))
    #G.add(Activation('sigmoid'))
    G.summary()

    return G
