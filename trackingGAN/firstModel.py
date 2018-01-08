import keras.backend as K
from keras.layers import (Input, Dense, Reshape, Flatten,
                          Dropout, BatchNormalization)
from keras.layers.advanced_activations import LeakyReLU
from keras.layers.convolutional import (UpSampling3D, Conv3D, ZeroPadding3D,
                                        AveragePooling3D)

from keras.models import Model, Sequential


# K.set_image_dim_ordering('tf')


def discriminator():
    Model = Sequential()
    dropoutratio = 0.2
    image = (500, 500, 500, 1)

    Model.add(Conv3D(16, 5, 5, 5, border_mode='same', input_shape=image))
    Model.add(LeakyReLU())
    Model.add(Dropout(dropoutratio))

    Model.add(ZeroPadding3D((2, 2, 2)))
    Model.add(Conv3D(8, 5, 5, 5, border_mode='valid'))
    Model.add(LeakyReLU(alpha=0.2))
    Model.add(BatchNormalization())
    Model.add(Dropout(0.2))
    #
    # Model.D.add(ZeroPadding3D((2, 2, 2))())
    # Model.D.add(Conv3D(8, 5, 5, 5, border_mode='valid')())
    # Model.D.add(LeakyReLU()())
    # Model.D.add(BatchNormalization()())
    # Model.D.add(Dropout(0.2)())
    #
    # Model.D.add(ZeroPadding3D((1, 1, 1))())
    # Model.D.add(Conv3D(8, 5, 5, 5, border_mode='valid')())
    # Model.D.add(LeakyReLU()())
    # Model.D.add(BatchNormalization()())
    # Model.D.add(Dropout(0.2)())
    #
    Model.add(AveragePooling3D(pool_size=(8, 8, 8)))
    # Model.add(Dense(1,activation='softmax'))
    # Model.add(Flatten())
    # Model.add(Dense(64,activation='relu'))
    Model.add(Flatten())
    Model.add(Dense(2, activation='softmax'))
    Model.summary()
    return Model
