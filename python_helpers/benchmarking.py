import os
import string
import itertools
import collections
import sys

def get_k_grams(text, k):
    k_grams = set()

    # Don't go out of bounds for an extra k gram
    final_char_index = len(text) - k + 1
    for i in range(0, final_char_index, k):
        k_gram = text[i:i + k]
        k_grams.add(k_gram.lower())
    return k_grams

def compare_files(file1, file2, k):
    with open(file1) as f1:
        text1 = f1.read()
    with open(file2) as f2:
        text2 = f2.read()
    k_grams1 = get_k_grams(text1, k)
    k_grams2 = get_k_grams(text2, k)
    # print(k_grams1)
    # print(k_grams2)

    common_k_grams = k_grams1 & k_grams2
    union_k_grams = k_grams1 | k_grams2
    similarity = len(common_k_grams) / len(union_k_grams)
    return similarity

def compare_all_files(folder, k):
    files = [os.path.join(folder, file) for file in os.listdir(folder)]
    similarities = collections.defaultdict(list)
    for file1, file2 in itertools.combinations(files, 2):
        similarity = compare_files(file1, file2, k)
        similarities[similarity].append((file1, file2))
    return similarities

def remove_newlines(folder_path):
    for filename in os.listdir(folder_path):
        filepath = os.path.join(folder_path, filename)
        if os.path.isfile(filepath):
            with open(filepath, 'r') as file:
                contents = file.read()
            with open(filepath, 'w') as file:
                contents = contents.replace('\n', '')
                file.write(contents)


k = int(sys.argv[1])
folder = f"../{sys.argv[2]}"
file_add_on = sys.argv[3]

if not file_add_on:
    file_add_on ='_'
else:
    file_add_on = '_' + file_add_on + '_'

print('Benchmarking from folder: ' + folder)
print('K:                        ' + str(k))
print('File add on:              ' + file_add_on)

threshold = 0.01
step = 0.01
max_threshold = 0.15

similarities = compare_all_files(folder, k)

prefix = "../outputs/benchmark_pairs" + file_add_on
print("Benchmarking files:       " + str(prefix))

while threshold <= max_threshold:
    filename = prefix + str(round(threshold, 2)) + ".txt"
    # print(filename)
    with open(filename, 'w') as f:
        for similarity, files in similarities.items():
            # print(similarity)
            # print(threshold)
            if similarity > threshold:
                for file1, file2 in files:
                    # print(file1)
                    # print(file2)
                    f.write(file1[3:])
                    f.write('\n')
                    f.write(file2[3:])
                    f.write('\n\n')
    
    threshold += step
    threshold = round(threshold, 3)

