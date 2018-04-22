import h5py
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


def plotTrackFromFile(file, rescale=True, delim=' '):
    x = np.loadtxt(file, delimiter=delim)
    if rescale:
        x = (x - 0.5) * 500
    fig = plt.figure().gca(projection='3d')
    # # ax = fig.add_subplot(111, projection='3d')
    fig.scatter(x[:, 0], x[:, 1], x[:, 2], s=3.4)
    # ax = fig.add_subplot(111)
    fig.annotate('$z[cm]$', xy=(0.9, 0.9), ha='left', va='top', xycoords='axes fraction', fontsize=11)
    fig.annotate('$x[cm]$', xy=(0.01, 0.23), ha='left', va='top', xycoords='axes fraction', fontsize=11)
    fig.annotate('$y[cm]$', xy=(0.93, 0.3), ha='left', va='top', xycoords='axes fraction', fontsize=11)

    fig.set_xlim([-250, 250])
    fig.set_ylim([-250, 250])
    fig.set_zlim([-250, 250])
    # fig.set_xlabel('x [mm]')#, fontsize=12)
    # fig.xaxis.set_label_coords(2.05, -0.025,0)
    # fig.set_ylabel('y [mm]')
    # fig.set_zlabel('z [mm]')
    plt.show()


def plotTrack(x, rescale=False):
    fig = plt.figure().gca(projection='3d')
    if rescale:
        x = (x - 0.5) * 500
    # # ax = fig.add_subplot(111, projection='3d')
    fig.scatter(x[:, 0], x[:, 1], x[:, 2], s=3.4)
    fig.annotate('$z[cm]$', xy=(0.9, 0.9), ha='left', va='top', xycoords='axes fraction', fontsize=11)
    fig.annotate('$x[cm]$', xy=(0.01, 0.23), ha='left', va='top', xycoords='axes fraction', fontsize=11)
    fig.annotate('$y[cm]$', xy=(0.93, 0.3), ha='left', va='top', xycoords='axes fraction', fontsize=11)
    fig.set_xlim([-250, 250])
    fig.set_ylim([-250, 250])
    fig.set_zlim([-250, 250])
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


def generateTransTrackTrainSet(scale=True, nrOfLevels=0):
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
    if nrOfLevels > 0:
        array = np.floor(array * nrOfLevels) / nrOfLevels
    array = array.reshape(array.shape[0], 159, 3, 1)
    if scale:
        array = array / 500 + 0.5
    return array


def generateTrainSetWithMomenta(dir='data/train_momenta/', scale=True, nrOfLevels=0):
    L = []
    M = []
    for file in os.listdir(dir):
        matrix = np.loadtxt(dir + file, delimiter=',')
        # if(m.shape[0]>159):
        #     continue
        m = matrix[0, :]
        p = matrix[1:, (0, 1, 2)]
        z = np.zeros((159 - p.shape[0], 3))
        p = np.concatenate((z, p), axis=0)
        # print(m.shape)
        L.append(p)
        M.append(m)
        # plotMatrix
    array = np.array(L)
    arrayM = np.array(M)
    if nrOfLevels > 0:
        array = np.floor(array * nrOfLevels) / nrOfLevels
    array = array.reshape(array.shape[0], 159, 3, 1)
    if scale:
        array = array / 500 + 0.5
    return array, arrayM


def saveDataset(dataset, momenta, name="data"):
    with h5py.File("data/" + name + '.h5', 'w') as hf:
        hf.create_dataset("data_p", data=dataset)
        if momenta.shape[0] > 0:
            hf.create_dataset("data_m", data=momenta)


def loadDataset(file_name="data/data.h5", momenta=True):
    with h5py.File(file_name, 'r') as hf:
        data = hf["data_p"][:]
        if momenta:
            momenta = hf["data_m"][:]
            return data, momenta
        else:
            return data


def saveImages(images, x, batch_size, orig=False):
    # print(x)
    if not os.path.exists('results/images_' + str(x)):
        os.makedirs('results/images_' + str(x))
    if orig:
        for i in range(batch_size):
            np.savetxt('results/images_' + str(x) + '/' + str(i) + 'o.txt', images[i])
    else:
        for i in range(batch_size):
            np.savetxt('results/images_' + str(x) + '/' + str(i) + '.txt', images[i])


def getDirection(dataset, scaled=True):
    L = []
    for data in dataset:
        i = 0
        while (not (data[i, 0] == data[i, 1] == data[i, 2] == 0.5)) and i < 158:
            i = i + 1
        # if scaled:
        #     while (not (data[i, 0] == data[i, 1] == data[i, 2] == 0.5)) and i < 158:
        #         i = i + 1
        #     else:
        #         while (not (data[i, 0] == data[i, 1] == data[i, 2] == 0)) and i < 158:
        #             i = i + 1
        L.append(-(data[0] - data[i - 1]))
    # data = data[range(i)]
    z = np.array(L)
    return np.reshape(z, (z.shape[0], 3))  # z[0], z[1], z[2]


def generateNoise(directions, size=75):
    L = []
    for direction in directions:
        direction = np.reshape(direction, (3, 1))
        # print(direction)
        try:
            x = size / 3
            x += 1
        except TypeError:
            "Noise Size must be divisible by 3"
        r = np.random.normal(0, 0.02, size=[size, 1])
        m = np.reshape(np.ones(size // 3) * direction, (size, 1)) + r
        L.append(m)
    x = np.array(L)
    return np.reshape(x, (x.shape[0], size))


def generateNoise5(directions, size=100):
    L = []
    for direction in directions:
        direction = np.reshape(direction, (5, 1))
        # print(direction)
        try:
            x = size / 5
            x += 1
        except TypeError:
            "Noise Size must be divisible by 5"
        r = np.random.normal(0, 0.02, size=[size, 1])
        m = np.reshape(np.ones(size // 5) * direction, (size, 1)) + r
        L.append(m)
    x = np.array(L)
    return np.reshape(x, (x.shape[0], size))
