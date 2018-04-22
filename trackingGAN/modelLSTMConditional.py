import keras.backend as K
from keras.layers import (Input, Dense, Reshape, Flatten, Lambda, merge,
                          Dropout, BatchNormalization, Activation, Embedding, Concatenate, LSTM)
from keras.layers.advanced_activations import LeakyReLU
from keras.layers.convolutional import (UpSampling2D, Conv2D, ZeroPadding2D, Conv2DTranspose,
                                        AveragePooling2D)

from keras.models import Model, Sequential

image_size = 159 * 3
image_shape = (None, 159, 3, 1)


def d_g_AM(input_size=75):
    d_x = Input(batch_shape=image_shape, name='imput1')

    d_r = Reshape((3, 159), input_shape=image_shape)(d_x)

    d_L1 = LSTM(159,return_sequences=True)(d_r)
    d_LF= Flatten()(d_L1)
    d_d1 = Dense(64, activation='relu')(d_LF)
    d_i2 = Input(batch_shape=(None, 3), name='input2')
    d_concat = Concatenate(axis=-1)([d_d1, d_i2])
    d_d2 = Dense(32, activation='relu')(d_concat)

    d_d3 = Dense(16, activation='relu')(d_d2)
    d_out = Dense(1, activation='sigmoid')(d_d3)

    d = Model([d_x, d_i2], d_out)

    # def generator(input_size=75):
    g_x = Input(shape=(input_size,))
    #g_f = Dense(100, activation='relu')(g_x)
    g_f2 = Dense(image_size)(g_x)
    g_fR = Reshape((3,159))(g_f2)

    g_L1 =LSTM(159, activation='linear',recurrent_activation='linear', return_sequences=True)(g_fR)

    g_out = Reshape((159, 3, 1))(g_L1)
    g = Model(g_x, g_out)

    # Sequential(Model(g_x,g_out),Model(inputs=[d_x,d_i2],outputs=d_out))
    AM_out = d([g(g_x),d_i2])
    AM = Model([g_x,d_i2],AM_out)
    #AM = Model(inputs=[g_x, d_i2], outputs=d_out)
    return d, g, AM
