import os
from time import sleep

directory_input = "input\\"
directory_output = "output\\"

directory = os.listdir(directory_input)

for filename in directory:
    command = ".\\main " + directory_input + filename + " " + directory_output + filename
    print(command)
    os.system(command)

