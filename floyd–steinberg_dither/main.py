import os
import sys
from time import sleep
from sklearn.cluster import KMeans
import numpy as np
import cv2
import math

directory_input = "input\\"
directory_output = "output\\"

directory = os.listdir(directory_input)

for filename in directory:
    kmeans_filename = 'kmeans_pallete\\' + filename.split('.')[0] + '.txt'
    try:
        kmeans_file = open(kmeans_filename, 'r')
    except:
        img = cv2.imread(directory_input + filename)
        if img is None:
            print('Erro abrindo a imagem.\n')
            sys.exit()
        img_array = []
        for y in range(img.shape[0]):
            for x in range(img.shape[1]):
                img_array.append([img[y][x][0], img[y][x][1], img[y][x][2]])
        kmeans = KMeans(n_clusters=32, random_state=0,
                        tol=0.001).fit(img_array)
        a_file = open(kmeans_filename, "w")
        for row in kmeans.cluster_centers_:
            a_file.write('{},{},{}\n'.format(int(row[0]), int(row[1]),
                                             int(row[2])))
    command = ".\\main " + directory_input + filename + " " + directory_output + filename + " " + kmeans_filename

    print(command)
    os.system(command)
