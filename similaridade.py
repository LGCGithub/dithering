# Rodar este arquivo no mesmo diretorio que está main.c e main.py

import os
import cv2 as cv
import numpy as np

barra = "" # tenho que testar em linux pra ver se funciona como eu espero que funcione

if os.name == 'nt':
    barra = "\\"
if os.name == 'posix':
    barra = "/"


directory_input = "input" + barra
directory_output = "output" + barra

directory = os.listdir(directory_input)

for filename in directory:
    # Carrega as imagens já na escala de cinza
    img_original = cv.imread(directory_input + filename, 0) 
    img_dithered = cv.imread(directory_output + filename, 0)

    img_original = img_original.astype(np.float32) / 255
    img_dithered = img_dithered.astype(np.float32) / 255

    h, w = img_original.shape

    # Calculo do coeficiente de correlação
    media_original = np.sum(img_original) / (h * w)
    media_dithered = np.sum(img_dithered) / (h * w)

    esp = np.sum((img_original - media_original) * (img_dithered - media_dithered))

    var_original = np.sum((img_original - media_original)**2) ** (0.5)
    var_dithered = np.sum((img_dithered - media_dithered)**2) ** (0.5)

    correlacao = esp / (var_original * var_dithered)

    # Borra a imagem que foi alterada
    img_original_borrada = cv.GaussianBlur(img_original, ksize=(7, 7), sigmaX=0)
    img_dithered_borrada = cv.GaussianBlur(img_dithered, ksize=(7, 7), sigmaX=0)

    # Recalcula correlação
    media_original_borrada = np.sum(img_original_borrada) / (h * w)
    media_dithered_borrada = np.sum(img_dithered_borrada) / (h * w)

    esp_borrada = np.sum((img_original_borrada - media_original_borrada) * (img_dithered_borrada - media_dithered_borrada))

    var_original_borrada = np.sum((img_original_borrada - media_original_borrada)**2) ** (0.5)
    var_dithered_borrada = np.sum((img_dithered_borrada - media_dithered_borrada)**2) ** (0.5)

    correlacao_borrada = esp_borrada / (var_original_borrada * var_dithered_borrada)


    # Mean squared error
    msq = np.sum((img_original - img_dithered)**2)
    msq_borrada = np.sum((img_original_borrada - img_dithered_borrada)**2)


    print("Imagem: " + filename)

    print("MSQ sem borrar: {:.4f}".format(msq), end="; ")
    print("MSQ borrando: {:.4f}".format(msq_borrada), end="\n")

    print("Correlacao sem borrar: {:.4f}".format(correlacao), end="; ")
    print("Correlacao borrando: {:.4f}".format(correlacao_borrada), end="\n\n")

    
    

    
