import random
import string
import os
import sys

documents_folder = 'DOCUMENTS'

# Setup params
input_folder = f"{documents_folder}/paraphrased_input_docs"
output_folder = f"{documents_folder}/paraphrased_output_docs"
percent_of_words_to_replace = 0.7
num_output_files_per_input = 10

if len(sys.argv) == 2:
    percent_of_words_to_replace = float(sys.argv[1])
elif len(sys.argv) == 3:
    percent_of_words_to_replace = float(sys.argv[1])
    num_output_files_per_input = int(sys.argv[2])


def generate_new_word(word):
    return ''.join(random.choice(string.ascii_lowercase) for _ in range(len(word)))

# For each file in the directory
for filename in os.listdir(input_folder):
    filepath = os.path.join(input_folder, filename)

    with open(filepath, 'r') as input_file:
        input_text = input_file.read()

        for i in range(0, num_output_files_per_input):
            output_file = f"{output_folder}/{filename[:-4]}-{i}.txt"
            words = input_text.split()
            num_words_to_replace = int(len(words) * percent_of_words_to_replace)
            words_to_replace = set(random.sample(words, num_words_to_replace))

            replaced_words = words.copy()
            already_replaced = set()
            skip = 10

            while len(already_replaced) < len(words_to_replace):
                for i in range(0, len(words), skip):
                    word = words[i]

                    if word in words_to_replace and word not in already_replaced:
                        new_word = generate_new_word(word)
                        replaced_words[i] = new_word
                        already_replaced.add(word)

                skip -= 1

            replaced_text = ' '.join(replaced_words)
            with open(output_file, 'w') as output_file:
                output_file.write(replaced_text)





