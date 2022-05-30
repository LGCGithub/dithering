import os
from time import sleep

directory_input = "input\\"
directory_output = "output\\"

directory = os.listdir(directory_input)

length = 8 # Valores: 2, 4, 8
bpp = 3 # Valores entre 1 até 8

for filename in directory:
    command = ".\\main " + directory_input + filename + " " + directory_output + filename + " " + str(length) + " " + str(bpp)
    print(command)
    os.system(command)

