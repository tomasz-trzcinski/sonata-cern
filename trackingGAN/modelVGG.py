import keras.backend as K
from keras.layers import (Input, Dense, Reshape, Flatten, Lambda, merge,
                          Dropout, BatchNormalization, Activation, Embedding, MaxPooling2D)
from keras.layers.advanced_activations import LeakyReLU
from keras.layers.convolutional import (UpSampling2D, Conv2D, ZeroPadding2D, Conv2DTranspose,
                                        AveragePooling2D)

from keras.models import Model, Sequential


def discriminator():
    model = Sequential()
    model.add(ZeroPadding2D((1, 1), input_shape=(159, 3, 1)))
    model.add(Conv2D(64, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(64, 3, 3, activation='relu'))
    model.add(MaxPooling2D((1, 1), strides=(2, 2)))

    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(128, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(128, 3, 3, activation='relu'))
    model.add(MaxPooling2D((1, 1), strides=(2, 2)))

    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(256, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(256, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(256, 3, 3, activation='relu'))
    model.add(MaxPooling2D((1, 1), strides=(2, 2)))

    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(512, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(512, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(512, 3, 3, activation='relu'))
    model.add(MaxPooling2D((1, 1)))

    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(512, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(512, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1, 1)))
    model.add(Conv2D(512, 3, 3, activation='relu'))
    model.add(MaxPooling2D((1, 1)))

    model.add(Flatten())
    model.add(Dense(4096, activation='relu'))
    model.add(Dropout(0.5))
    model.add(Dense(4096, activation='relu'))
    model.add(Dropout(0.5))
    model.add(Dense(1, activation='sigmoid'))
    model.summary()
    return model


def generator():
    model = Sequential()
    inputDim = 100
    depth = 159 * 3
    # netDepth=8
    model.add(Dense(4096, input_shape=(inputDim,)))
    model.add(Dropout(0.5))
    model.add(Dense(4096, activation='relu'))
    model.add(Dropout(0.5))
    model.add(Dense(depth))
    model.add(Reshape((159, 3, 1)))

    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(MaxPooling2D((1, 1)))
    # G.add(UpSampling2D())

    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(MaxPooling2D((1, 1)))
    # G.add(UpSampling2D())

    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(MaxPooling2D((1, 1)))

    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(512, 3, padding='same', activation='relu'))
    model.add(MaxPooling2D((1, 1)))

    model.add(Conv2DTranspose(256, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(256, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(256, 3, padding='same', activation='relu'))
    model.add(MaxPooling2D((1, 1)))

    model.add(Conv2DTranspose(128, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(128, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(128, 3, padding='same', activation='relu'))
    model.add(MaxPooling2D((1, 1)))

    model.add(Conv2DTranspose(64, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(64, 3, padding='same', activation='relu'))
    model.add(Conv2DTranspose(64, 3, padding='same', activation='relu'))
    model.add(MaxPooling2D((1, 1)))

    model.add(Conv2DTranspose(1, 3, padding='same'))
    # G.add(Activation('sigmoid'))
    model.summary()

    return model
