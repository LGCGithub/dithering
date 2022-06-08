import os
from time import sleep

directory_input = "input\\"
directory_output = "output\\"

directory = os.listdir(directory_input)

length = 4 # Valores: 2, 4, 8
size = 64 # Tamanho da paleta
mode = 1 # 1 == EGA pallete, 2 == random pallete

for filename in directory:
    command = ".\\main " + directory_input + filename + " " + directory_output + filename + " " + str(length) + " " + str(mode) + " " + str(size)
    print(command)
    os.system(command)

