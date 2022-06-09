import os
from time import sleep

barra = "" # tenho que testar em linux pra ver se funciona como eu espero que funcione

if os.name == 'nt':
    barra = "\\"
if os.name == 'posix':
    barra = "/"


directory_input = "input" + barra
directory_output = "output" + barra

directory = os.listdir(directory_input)

length = 8 # Valores: 2, 4, 8
size = 64 # Tamanho da paleta
mode = 3 # 1 == EGA pallete, 2 == random pallete, 3 == k-means pallete
interacoes = 5


for filename in directory:
    command = "." + barra + "main " + directory_input + filename + " " + directory_output + filename + " " + str(length) + " " + str(mode) + " " + str(size) + " " + str(interacoes)
    print(command)
    os.system(command)