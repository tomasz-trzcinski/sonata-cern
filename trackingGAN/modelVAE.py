import numpy as np
from keras.layers import Input, Dense, Lambda, Layer, Flatten, Reshape, Conv2DTranspose, Activation, LeakyReLU, Dropout
from keras.models import Model, Sequential, save_model
from keras import backend as K, objectives, optimizers
from keras import metrics

import dataPreprocessing



def trainVAE(epochs=100):
    batch_size = 32
    original_dim = 159 * 3
    latent_dim = 100
    intermediate_dim = 200
    epsilon_std = 1.0

    # image = (159, 3, 1)

    x = Input(shape=(original_dim,))
    h = Dense(intermediate_dim, activation='relu')(x)
    z_mean = Dense(latent_dim)(h)
    z_log_sigma = Dense(latent_dim)(h)


    def sampling(args):
        z_mean, z_log_var = args
        epsilon = K.random_normal(shape=(K.shape(z_mean)[0], latent_dim), mean=0.,
                                  stddev=epsilon_std)
        return z_mean + K.exp(z_log_var / 2) * epsilon


    z = Lambda(sampling, output_shape=(latent_dim,))([z_mean, z_log_sigma])

    decoder_h = Dense(intermediate_dim, activation='relu')
    decoder_mean = Dense(original_dim, activation='sigmoid')
    h_decoded = decoder_h(z)
    x_decoded_mean = decoder_mean(h_decoded)
    # x_decoded_mean_reshaped = Reshape((159, 5, 1))(x_decoded_mean)

    # autoencoder
    vae = Model(x, x_decoded_mean)

    # generator



    # def vae_loss(y_true, y_pred):
    #     """ Calculate loss = reconstruction loss + KL loss for each data in minibatch """
    #     # E[log P(X|z)]
    #     recon = K.sum(K.binary_crossentropy(y_pred, y_true), axis=1)
    #     # D_KL(Q(z|X) || P(z|X)); calculate in closed form as both dist. are Gaussian
    #     kl = 0.5 * K.sum(K.exp(z_log_sigma) + K.square(z_mean) - 1. - z_log_sigma, axis=1)
    #     return recon + kl

    def vae_loss(y_true, y_pred):
        """ Calculate loss = reconstruction loss + KL loss for each data in minibatch """
        # E[log P(X|z)]
        recon = original_dim *metrics.mean_squared_error(x, x_decoded_mean)
        # D_KL(Q(z|X) || P(z|X)); calculate in closed form as both dist. are Gaussian
        kl = 0.3 * K.sum(K.exp(z_log_sigma) + K.square(z_mean) - 1. - z_log_sigma, axis=1)
        return recon + kl


    # def vae_loss(x, x_decoded_mean):
    #     xent_loss = objectives.binary_crossentropy(x, x_decoded_mean)
    #     kl_loss = - 0.5 * K.mean(1 + z_log_sigma - K.square(z_mean) - K.exp(z_log_sigma), axis=-1)
    #
    #     return xent_loss + kl_loss


    rms_opt = optimizers.rmsprop(lr=0.0001)
    adam_opt = optimizers.adam(lr=0.001)
    vae.compile(optimizer=adam_opt, loss=vae_loss)

    data = dataPreprocessing.generateTransTrackTrainSet()
    x_train = np.reshape(data, (data.shape[0], original_dim))
    x_train = x_train / 500 + 0.5

    vae.fit(x_train, x_train, shuffle=True,
            epochs=epochs,
            batch_size=batch_size,
            validation_split=0.3)

    decoder_input = Input(shape=(latent_dim,))
    _h_decoded = decoder_h(decoder_input)
    _x_decoded_mean = decoder_mean(_h_decoded)
    x_decoded_mean_reshaped = Reshape((159, 3, 1))(_x_decoded_mean)
    generator = Model(decoder_input, x_decoded_mean_reshaped)

    dataPreprocessing.saveplotTrack(np.reshape(x_train[0], (159, 3)),"vae/original.png")
    dataPreprocessing.saveplotTrack(np.reshape(vae.predict(np.reshape(x_train[0], (1, original_dim))), (159, 3)),"vae/encoded.png")
    save_model(generator,"vae_gen.h5")
    save_model(vae,"vae.h5")
    # print((x_train[0]-0.5)*500)
    # print((vae.predict(np.reshape(x_train[0], (1, original_dim)))-0.5)*500)
    return generator


def trainVAEForGAN(epochs=50):
    batch_size = 32
    original_dim = 159 * 3
    latent_dim = 100
    intermediate_dim = 200
    epsilon_std = 1.0

    # image = (159, 3, 1)

    x = Input(shape=(original_dim,))
    h = Dense(intermediate_dim, activation='relu')(x)
    z_mean = Dense(latent_dim)(h)
    z_log_sigma = Dense(latent_dim)(h)

    def sampling(args):
        z_mean, z_log_var = args
        epsilon = K.random_normal(shape=(K.shape(z_mean)[0], latent_dim), mean=0.,
                                  stddev=epsilon_std)
        return z_mean + K.exp(z_log_var / 2) * epsilon


    z = Lambda(sampling, output_shape=(latent_dim,))([z_mean, z_log_sigma])

    decoder_h = Dense(intermediate_dim, activation='relu')
    decoder_mean = Dense(original_dim, activation='sigmoid')

    conv1 = Conv2DTranspose(original_dim // 2, (15, 3), padding='same')
    conv2 = Conv2DTranspose(original_dim // 2, (10, 3), padding='same')
    conv3 = Conv2DTranspose(original_dim // 2, (15, 3), padding='same')
    outConv = Conv2DTranspose(1, 3, padding='same')

    #Generator VAE
    h_decoded = decoder_h(z)
    x_decoded_mean = decoder_mean(h_decoded)
    x_decoded_mean_reshaped = Reshape((159, 3, 1))(x_decoded_mean)
    c1 = conv1(x_decoded_mean_reshaped)
    c1_a = LeakyReLU()(c1)

    #c2 = conv2(c1_a)
    #c2_a = LeakyReLU()(c2)

    c3 = conv3(c1_a)
    c3_a = LeakyReLU()(c3)
    c3_ad = Dropout(0.4)(c3_a)
    out = outConv(c3_ad)
    out_reshaped = Flatten()(out)
    #out_reshaped = Reshape((original_dim,1))(outF)
    #out_reshaped = Reshape((1, original_dim))(out1)

    # autoencoder
    vae = Model(x, out_reshaped)

    #Generator final
    decoder_input = Input(shape=(latent_dim,))
    _h_decoded = decoder_h(decoder_input)
    _x_decoded_mean = decoder_mean(_h_decoded)
    _x_decoded_mean_reshaped = Reshape((159, 3, 1))(_x_decoded_mean)
    _c1 = conv1(_x_decoded_mean_reshaped)
    _c1_a = LeakyReLU()(_c1)

    #_c2 = conv2(_c1_a)
    #_c2_a = LeakyReLU()(_c2)

    _c3 = conv3(_c1_a)
    _c3_a = LeakyReLU()(_c3)
    _c3_ad = Dropout(0.4)(_c3_a)
    _out = outConv(_c3_ad)
    #_out_reshaped = Flatten()(_out)
    #_out_reshaped = Reshape((original_dim,1))(_outF)
    #_out_reshaped = Reshape((1, original_dim))(_out1)

    generator = Model(decoder_input, _out)


    def vae_loss(y_true, y_pred):
        """ Calculate loss = reconstruction loss + KL loss for each data in minibatch """
        # E[log P(X|z)]
        recon = original_dim *metrics.mean_squared_error(x, out_reshaped)
        # D_KL(Q(z|X) || P(z|X)); calculate in closed form as both dist. are Gaussian
        kl = 0.5 * K.sum(K.exp(z_log_sigma) + K.square(z_mean) - 1. - z_log_sigma, axis=1)
        return recon + kl

    rms_opt = optimizers.rmsprop(lr=0.0001)
    adam_opt = optimizers.adam(lr=0.002, decay=3e-7)
    vae.compile(optimizer=adam_opt, loss=vae_loss)

    data = dataPreprocessing.generateTransTrackTrainSet(True)
    x_train = np.reshape(data, (data.shape[0], original_dim))
    x_train = x_train / 500 + 0.5

    vae.fit(x_train, x_train, shuffle=True,
            epochs=epochs,
            batch_size=batch_size,
            validation_split=0.3)



    dataPreprocessing.saveplotTrack(np.reshape(x_train[0], (159, 3)),"vae/original.png")
    dataPreprocessing.saveplotTrack(np.reshape(vae.predict(np.reshape(x_train[0], (1, original_dim))), (159, 3)),"vae/encoded.png")
    save_model(generator,"models/vae_gen.h5")
    save_model(vae,"models/vae.h5")
    # print((x_train[0]-0.5)*500)
    # print((vae.predict(np.reshape(x_train[0], (1, original_dim)))-0.5)*500)
    return generator

# trainVAEForGAN(100)