import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import os


def convertToMatrix(filename):
    data = np.loadtxt(filename, delimiter=',')
    dataInt = data.astype(int)
    # np.asmatrix(dataInt)
    dataMatrix = np.zeros((500, 500, 500))
    for row in dataInt:
        dataMatrix[row[0] + 250, row[1] + 250, row[2] + 250] = row[3]
    return dataMatrix


def plotTrackFromFile(file):
    x = np.loadtxt(file)
    fig = plt.figure().gca(projection='3d')
    # # ax = fig.add_subplot(111, projection='3d')
    fig.scatter(x[:, 0], x[:, 1], x[:, 2], s=3.4)
    plt.show()


def plotTrack(x):
    fig = plt.figure().gca(projection='3d')
    # # ax = fig.add_subplot(111, projection='3d')
    fig.scatter(x[:, 0], x[:, 1], x[:, 2], s=3.4)
    plt.show()


def saveplotTrack(x, name):
    fig = plt.figure().gca(projection='3d')
    # # ax = fig.add_subplot(111, projection='3d')
    fig.scatter(x[:, 0], x[:, 1], x[:, 2], s=3.4)
    plt.savefig(name)


def plotMatrix(dataMatrix):
    z, x, y = dataMatrix.nonzero()
    fig = plt.figure().gca(projection='3d')
    # ax = fig.add_subplot(111, projection='3d')
    fig.scatter(x, y, z, s=0.5)
    plt.show()


def generateTrainSet():
    L = []
    for file in os.listdir('data/train'):
        m = convertToMatrix('data/train/' + file)
        # m.reshape(1,500,500,500)
        L.append(m)
        # plotMatrix(m)
    return np.array(L)


def generateTransTrainSet():
    L = []
    for file in os.listdir('data/train'):
        m = np.loadtxt('data/train/' + file, delimiter=',')
        # if(m.shape[0]>159):
        #     continue
        z = np.zeros((159 - m.shape[0], 5))
        m = np.concatenate((m, z), axis=0)
        # print(m.shape)
        L.append(m)
        # plotMatrix
    array = np.array(L)
    return array.reshape(array.shape[0], 159, 5, 1)


def generateTransTrackTrainSet(scale=False, nrOfLevels=0):
    L = []
    for file in os.listdir('data/train'):
        m = np.loadtxt('data/train/' + file, delimiter=',')
        # if(m.shape[0]>159):
        #     continue
        m = m[:, (0, 1, 2)]
        z = np.zeros((159 - m.shape[0], 3))
        m = np.concatenate((m, z), axis=0)
        # print(m.shape)
        L.append(m)
        # plotMatrix
    array = np.array(L)
    if scale:
        array = array.reshape(array.shape[0], 159, 3, 1) / 500 + 0.5
    else:
        array = array.reshape(array.shape[0], 159, 3, 1)

    if nrOfLevels > 0:
        array = np.floor(array * nrOfLevels) / nrOfLevels

    return array


def saveImages(images, x, batch_size):
    # print(x)
    if not os.path.exists('results/images_' + str(x)):
        os.makedirs('results/images_' + str(x))
    for i in range(batch_size):
        np.savetxt('results/images_' + str(x) + '/' + str(i) + '.txt', images[i])
