import os
import sys

def remove_newlines(folder_path):
    for filename in os.listdir(folder_path):
        print(filename)
        filepath = os.path.join(folder_path, filename)
        if os.path.isfile(filepath):
            with open(filepath, 'r') as file:
                contents = file.read()
            with open(filepath, 'w') as file:
                contents = contents.replace('\n', '')
                file.write(contents)

if len(sys.argv) > 1:
    remove_newlines(sys.argv[1])