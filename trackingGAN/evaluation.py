import numpy as np
import os
import scipy as spy
import time

import_time = time.time()
from keras.models import load_model

import dataPreprocessing
import matplotlib.pyplot as plt

import modelTrans
import modelTransPathSmall

#print('Import time ', time.time() - import_time)
os.environ["CUDA_VISIBLE_DEVICES"] = "1"


def get_results(directory='results/eval/'):
    L = []
    l = os.listdir(directory)
    for folder in l:
        if os.path.isdir(directory + folder):
            files = os.listdir(directory + folder)
            for file in files:
                # print(file)
                if os.path.isdir(directory + folder + '/' + file):
                    continue
                # print('results/' + folder + '/' + file)
                m = np.loadtxt(directory + folder + '/' + file, delimiter=' ')
                L.append(m)
    # print(L.__len__())
    # for file in os.listdir('data/train'):
    #     m = np.loadtxt('data/train/' + file, delimiter=',')
    #     # m = m[:, (0, 1, 2)]
    #     # z = np.zeros((159 - m.shape[0], 3))
    #     # m = np.concatenate((m, z), axis=0)
    #     # print(m.shape)
    #     L.append(m)
    #     # plotMatrix
    array = np.array(L)
    return array


def circle_equation(A, B, C):
    a = np.linalg.norm(C - B)
    b = np.linalg.norm(C - A)
    c = np.linalg.norm(B - A)
    s = (a + b + c) / 2
    R = a * b * c / 4 / np.sqrt(s * (s - a) * (s - b) * (s - c))
    b1 = a * a * (b * b + c * c - a * a)
    b2 = b * b * (a * a + c * c - b * b)
    b3 = c * c * (a * a + b * b - c * c)
    P = np.column_stack((A, B, C)).dot(np.hstack((b1, b2, b3)))
    P /= b1 + b2 + b3
    return R, P


def computeCircleError(data, scaled=False):
    i = 0
    # if scaled:
    #     while (not (data[i, 0] == data[i, 1] == data[i, 2] == 0.5)) and i < 158:
    #         i = i + 1
    #     else:
    while (not (data[i, 0] == data[i, 1] == data[i, 2] == 0.5)) and i < 158:
        i = i + 1
    # print(i)
    data = data[range(i)]
    r, p = circle_equation(data[0], data[i // 2], data[i - 1])
    print("R",np.linalg.norm(r - np.linalg.norm(p - data)))
    p = np.reshape(p, (3, 1))
    sum = 0
    for j in range(i):
        sum += np.linalg.norm(r - np.linalg.norm(p - data[j]))
    if np.isnan(sum):
        sum = 0
    return sum


def circleLoss(data):
    L = []
    for i in range(data.shape[0]):
        x = computeCircleError(data[i])
        L.append(x)
    return L


def computeError(data):
    if data.shape[0] == 0:
        return 75
    sum = 0
    for i in range(data.shape[0]):
        x = computeCircleError(data[i])
        # print(x)
        sum += x
    return sum / data.shape[0]


def findBests(directory='results/', distance=20):
    l = os.listdir(directory)
    for folder in l:
        if os.path.isdir(directory + folder):
            files = os.listdir(directory + folder)
            L = []
            for file in files:
                # print(file)
                if os.path.isdir(directory + folder + '/' + file):
                    continue
                # print('results/' + folder + '/' + file)
                m = np.loadtxt(directory + folder + '/' + file, delimiter=' ')
                L.append(m)
            error = computeError(np.array(L))
            if error < 30:
                print(folder, error)
    # print(L.__len__())
    # for file in os.listdir('data/train'):
    #     m = np.loadtxt('data/train/' + file, delimiter=',')
    #     # m = m[:, (0, 1, 2)]
    #     # z = np.zeros((159 - m.shape[0], 3))
    #     # m = np.concatenate((m, z), axis=0)
    #     # print(m.shape)
    #     L.append(m)
    #     # plotMatrix
    # array = np.array(L)
    # return array


def generateTracks(g, noiseSize=100, nrOfSamples=10, save=True):
    # noise = np.random.uniform(0, 1, size=[nrOfSamples, noiseSize])
    noise = np.random.normal(1, 0.2, size=[nrOfSamples, noiseSize])
    pred_time = time.time()
    images = g.predict(noise)
    after_pred_time = time.time()
    print('Pred time: ', after_pred_time - pred_time)
    if save:
        for i in range(nrOfSamples):
            np.savetxt('results/generated/image_' + str(i) + '.txt', images[i])
    return images


def evaluate():
    # g = load_model('models/g_scaled_Large300.h5')
    # g = load_model('models/g_scaled_Large200.h5')  # average error on input data:  0.830432364988 # po doskalowaniu average error on input data:  6.85097328978
    # g = load_model('models/g_scaled_Large.h5') #average error on input data:  75.8611711547
    # g = load_model('models/g_scaled_trans2.h5') #average error on input data:  8,32608332227
    # g = load_model('models/g_pro_5.h5') #average error on input data:  0.279906307591
    # g = load_model('models/g_MLP.h5') #average error on input data:  17,6137669419
    # g = load_model('models/cvae_gen.h5')
    # g = load_model('models/vae_gen.h5')
    g = load_model('models/g_LSTM.h5')

    g.summary()
    # g = load_model('models/cvae_gen.h5') #MLP vae average error on input data:  3.34077954669
    start_time = time.time()
    d = generateTracks(g, nrOfSamples=100, save=False)
    elapsed_time = time.time() - start_time
    print('Generation time:', elapsed_time)

    # print(d.shape)
    # d = np.random.uniform(0, 1, size=[1000, 159, 3, 1]) #average error on input data:  33.830432364988
    # d=(d+0.5)/2#*10-4.5
    for i in range(3):
        dataPreprocessing.plotTrack(d[i])  # plotTrackFromFile('results/generated/image_'+str(i)+'.txt')
    # d = dataPreprocessing.generateTransTrackTrainSet(scale=True)
    # r = get_results('results/eval/')
    # print('sum', computeCircleError(d[1]))
    print('average error on input data: ',
          computeError(d) / 159)  # ('average error on input data: ', 0.027225501561514154)
    # not scaled('average error on input data: ', 13.612750778167442)
    # print('average error on generated data: ', computeError(r))
    # findBests('results/')

computeCircleError(dataPreprocessing.generateTransTrackTrainSet(scale=True)[0])