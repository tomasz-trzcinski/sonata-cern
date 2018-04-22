import keras.backend as K
from keras.layers import (Input, Dense, Reshape, Flatten, Lambda, merge,
                          Dropout, BatchNormalization, Activation, Embedding, Concatenate)
from keras.layers.advanced_activations import LeakyReLU
from keras.layers.convolutional import (UpSampling2D, Conv2D, ZeroPadding2D, Conv2DTranspose,
                                        AveragePooling2D, Cropping2D)

from keras.models import Model, Sequential

image_size = 159 * 3
image_shape = (None, 159, 3, 1)

def generator(input_size = 99):
    g_x = Input(shape=(input_size,))
    # g_f = Dense(100, activation='relu')(g_x)
    g_f2 = Dense(input_size)(g_x)
    g_fR = Reshape((33, 3, 1))(g_f2)

    g_c1 = Conv2DTranspose(50, (20, 3), padding='valid')(g_fR)
    g_c1A = LeakyReLU()(g_c1)

    #g_c2U = UpSampling2D((2,1))(g_c1A)
    g_c2 = Conv2DTranspose(30, (25, 3), padding='valid')(g_c1A)
    g_c2A = LeakyReLU()(g_c2)
    g_c2B = BatchNormalization()(g_c2A)

    g_c3U = UpSampling2D((2, 1))(g_c2B)
    g_c3 = Conv2DTranspose(40, (20, 3), padding='valid')(g_c3U)
    g_c3A = LeakyReLU()(g_c3)
    g_c3B = BatchNormalization()(g_c3A)

    #g_c4U = UpSampling2D((3,1))(g_c3B)
    g_c4D = AveragePooling2D((1,3))(g_c3B)
    g_c4C = Cropping2D(cropping=((6,6),(0,0)))(g_c4D)
    g_c4 = Conv2DTranspose(30, (40, 3), padding='same')(g_c4C)
    g_c4A = LeakyReLU()(g_c4)
    g_c4B = BatchNormalization()(g_c4A)

    g_out = Conv2DTranspose(1, (159, 3), padding='same', activation='linear')(g_c4B)
    g = Model(g_x, g_out)
    g.summary()
    return g

def discriminator():
    d_x = Input(batch_shape=image_shape, name='imput1')
    d_c1 = Conv2D(150, (40, 3), padding='same')(d_x)
    d_c1A = LeakyReLU()(d_c1)

    d_c2 = Conv2D(70, (25, 3),name="Conv2D")(d_c1A)
    d_c2A = LeakyReLU()(d_c2)
    d_c2AB = BatchNormalization()(d_c2A)

    d_c3 = Conv2D(50, (20, 1))(d_c2AB)
    d_c3A = LeakyReLU()(d_c3)
    d_c3AB = BatchNormalization()(d_c3A)
    d_c3AP = AveragePooling2D((5,1))(d_c3AB)

    d_c4 = Conv2D(30, (20, 1))(d_c3AP)
    d_c4A = LeakyReLU()(d_c4)
    #d_c4AP = AveragePooling2D((25,3))(d_c4A)

    d_c3AP = AveragePooling2D((5,1))(d_c3A)

    d_c4 = Conv2D(10, (10, 1))(d_c3AP)
    d_c4A = LeakyReLU()(d_c4)

    d_f = Flatten()(d_c4A)

    d_d1 = Dense(32, activation='relu')(d_f)
    d_out = Dense(1, activation='linear')(d_d1)

    d = Model(d_x, d_out)
    return d
