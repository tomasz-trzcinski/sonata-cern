import keras.backend as K
from keras.layers import (Input, Dense, Reshape, Flatten, Lambda, merge,
                          Dropout, BatchNormalization, Activation, Embedding)
from keras.layers.advanced_activations import LeakyReLU
from keras.layers.convolutional import (UpSampling2D, Conv2D, ZeroPadding2D, Conv2DTranspose,
                                        AveragePooling2D)

from keras.models import Model, Sequential


# K.set_image_dim_ordering('tf')


def discriminator():
    D = Sequential()
    dropoutratio = 0.3
    image = (159, 5, 1)

    D.add(Flatten(input_shape=image))
    D.add(Dense(1028))
    #D.add(Conv2D(64, 5, 5, border_mode='same', input_shape=image))
    D.add(LeakyReLU())
    D.add(Dropout(0.2))

    D.add(Dense(256))
    #D.add(ZeroPadding2D((2, 2)))
    ##D.add(Conv2D(32, 5, 5, border_mode='valid'))
    D.add(LeakyReLU(alpha=0.2))
    D.add(BatchNormalization())
    D.add(Dropout(dropoutratio))

    #D.add(ZeroPadding2D((2, 2)))
    #D.add(Conv2D(32, 5, 5, border_mode='valid'))
    D.add(LeakyReLU(alpha=0.2))
    D.add(BatchNormalization())
    D.add(Dropout(dropoutratio))
    #
    # D.D.add(ZeroPadding3D((1, 1, 1))())
    # D.D.add(Conv3D(8, 5, 5, 5, border_mode='valid')())
    # D.D.add(LeakyReLU()())
    # D.D.add(BatchNormalization()())
    # D.D.add(Dropout(0.2)())
    #
    # D.add(AveragePooling2D(pool_size=(2, 2)))
    # D.add(Dense(1,activation='softmax'))
    # D.add(Flatten())
    # D.add(Dense(64,activation='relu'))
    #D.add(Flatten())
    D.add(Dense(64))
    D.add(Activation('relu'))
    D.add(Dense(1))
    D.add(Activation('sigmoid'))
    D.summary()
    return D


def generator():
    G = Sequential()
    inputDim = 100
    depth = 159 * 5
    # netDepth=8
    G.add(Dense(depth, input_shape=(inputDim,)))
    G.add(Activation('relu'))


    G.add(Dense(2*depth))
    #G.add(Conv2DTranspose(depth // 2, 5, padding='same'))
    G.add(BatchNormalization())
    G.add(Activation('relu'))
    #G.add(UpSampling2D())

    G.add(Dense(depth))
    #G.add(BatchNormalization(momentum=0.9))
    G.add(Activation('relu'))
    G.add(Dropout(0.4))
    #G.add(UpSampling2D())

    # G.add(Dense(int(depth // 8)))
    # #G.add(BatchNormalization(momentum=0.9))
    # G.add(Activation('relu'))
    # G.add(Dropout(0.4))

    #G.add(Conv2DTranspose(1, 5, padding='same'))
    G.add(Dense(depth))
    G.add(Reshape((159, 5, 1)))

    #G.add(Activation('sigmoid'))
    G.summary()

    return G
