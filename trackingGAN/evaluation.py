import numpy as np
import os
import scipy as spy
import dataPreprocessing
import matplotlib.pyplot as plt


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


def computeCircleError(data):
    # dataPreprocessing.plotTrack(data)
    # fig = plt.figure().gca(projection='3d')
    # # ax = fig.add_subplot(111, projection='3d')
    # fig.scatter(data[:, 0], data[:, 1], data[:, 2], s=3.4)
    i = 0
    while (not (data[i, 0] == data[i, 1] == data[i, 2] == 0)) and i < 158:
        i = i + 1
        # print(i)
    data = data[range(i)]
    r, p = circle_equation(data[0], data[i // 2], data[i - 1])
    p = np.reshape(p, (3, 1))
    sum = 0
    for j in range(i):
        sum += np.linalg.norm(r - np.linalg.norm(p - data[j]))
    if np.isnan(sum):
        sum = 0
    return sum

    # fig = plt.figure()
    # ax = fig.add_subplot(111, projection='3d')
    # # ax.set_aspect('equal')
    #
    # u = np.linspace(0, 2 * np.pi, 100)
    # v = np.linspace(0, np.pi, 100)
    #
    # x = r * np.outer(np.cos(u), np.sin(v))
    # y = r * np.outer(np.sin(u), np.sin(v))
    # z = r * np.outer(np.ones(np.size(u)), np.cos(v))
    # # for i in range(2):
    # #    ax.plot_surface(x+random.randint(-5,5), y+random.randint(-5,5), z+random.randint(-5,5),  rstride=4, cstride=4, color='b', linewidth=0, alpha=0.5)
    # ax.plot_surface(x, y, z, rstride=4, cstride=4, color='b', linewidth=0, alpha=0.5)
    # plt.show()

    # data2 = np.linalg.svd(data)
    # print(data2[1], data[2])
    # dataPreprocessing.plotTrack(data2)


def computeError(data):
    if data.shape[0]==0:
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


# d = dataPreprocessing.generateTransTrackTrainSet(scale=True)
# r = get_results('results/')
# print('sum', computeCircleError(d[1]))
# print('average error on input data: ', computeError(d)) #('average error on input data: ', 0.72681844664185569)
# print('average error on generated data: ', computeError(r))
findBests('results/')
