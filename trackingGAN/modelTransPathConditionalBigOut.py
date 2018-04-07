import keras.backend as K
from keras.layers import (Input, Dense, Reshape, Flatten, Lambda, merge,
                          Dropout, BatchNormalization, Activation, Embedding, Concatenate)
from keras.layers.advanced_activations import LeakyReLU
from keras.layers.convolutional import (UpSampling2D, Conv2D, ZeroPadding2D, Conv2DTranspose,
                                        AveragePooling2D)

from keras.models import Model, Sequential

image_size = 159 * 3
image_shape = (None, 159, 3, 1)


def d_g_AM(input_size=75):
    d_x = Input(batch_shape=image_shape, name='imput1')
    d_c1 = Conv2D(150, (40, 3), padding='same')(d_x)
    d_c1A = LeakyReLU()(d_c1)

    d_c2 = Conv2D(70, (25, 3), name="Conv2D")(d_c1A)
    d_c2A = LeakyReLU()(d_c2)
    d_c2AB = BatchNormalization()(d_c2A)

    d_c3 = Conv2D(50, (20, 1))(d_c2AB)
    d_c3A = LeakyReLU()(d_c3)
    d_c3AB = BatchNormalization()(d_c3A)
    d_c3AP = AveragePooling2D((5, 1))(d_c3AB)

    d_c4 = Conv2D(30, (20, 1))(d_c3AP)
    d_c4A = LeakyReLU()(d_c4)
    # d_c4AP = AveragePooling2D((25,3))(d_c4A)

    d_c3AP = AveragePooling2D((5, 1))(d_c3A)

    d_c4 = Conv2D(10, (10, 1))(d_c3AP)
    d_c4A = LeakyReLU()(d_c4)

    d_f = Flatten()(d_c4A)

    d_d1 = Dense(32, activation='relu')(d_f)
    d_i2 = Input(batch_shape=(None, 3), name='input2')
    d_concat = Concatenate(axis=-1)([d_d1, d_i2])
    d_d2 = Dense(64, activation='relu')(d_concat)

    d_d3 = Dense(32, activation='relu')(d_d2)
    d_out = Dense(1, activation='sigmoid')(d_d3)

    d = Model([d_x, d_i2], d_out)

    # def generator(input_size=75):
    g_x = Input(shape=(input_size,))
    # g_f = Dense(100, activation='relu')(g_x)
    g_f2 = Dense(image_size)(g_x)
    g_fR = Reshape((159, 3, 1))(g_f2)

    g_c1 = Conv2DTranspose(100, (30, 3), padding='same')(g_fR)
    g_c1A = LeakyReLU()(g_c1)

    g_c2 = Conv2DTranspose(60, (25, 3), padding='same', name="Conv2G")(g_c1A)
    g_c2A = LeakyReLU()(g_c2)
    g_c2B = BatchNormalization()(g_c2A)

    g_c3 = Conv2DTranspose(80, (20, 3), padding='same')(g_c2B)
    g_c3A = LeakyReLU()(g_c3)
    g_c3B = BatchNormalization()(g_c3A)

    g_c4 = Conv2DTranspose(60, (30, 3), padding='same')(g_c3B)
    g_c4A = LeakyReLU()(g_c4)
    g_c4B = BatchNormalization()(g_c4A)

    g_c4 = Conv2DTranspose(30, (40, 3), padding='same')(g_c3B)
    g_c4A = LeakyReLU()(g_c4)
    g_c4B = BatchNormalization()(g_c4A)

    g_out = Conv2DTranspose(1, (159, 3), padding='same',name ="g_out")(g_c4B)
    g = Model(g_x, g_out)
    g.summary()
    d.summary()
    # Sequential(Model(g_x,g_out),Model(inputs=[d_x,d_i2],outputs=d_out))
    g_out2 = g(g_x)
    AM_out = d([g_out2, d_i2])
    AM = Model([g_x, d_i2], [AM_out, g_out])
    # AM = Model(inputs=[g_x, d_i2], outputs=d_out)
    return d, g, AM  # , g_out
