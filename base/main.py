import cv2 as cv
import numpy as np
import os


def dither(img):
    # ...
    return img


directory_input = "input\\"
directory_output = "output\\"

directory = os.listdir(directory_input)

for filename in directory:
    img = cv.imread(directory_input + filename)
    dithered = dither(img)
    cv.imwrite(directory_output + filename,
               dithered)  # Multiplicar por 255 caso seja float
