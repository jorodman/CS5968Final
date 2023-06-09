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

    common_k_grams = k_grams1 & k_grams2
    union_k_grams = k_grams1 | k_grams2
    similarity = len(common_k_grams) / len(union_k_grams)
    return similarity

def compare_all_files(files, k):
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


LSH_k = int(sys.argv[1])
folder = f"../{sys.argv[2]}"
max_files = int(sys.argv[3])

filename = ''

if len(sys.argv) > 4:
    filename ='../outputs/benchmark_test_docs.txt'
else:
    filename = "../outputs/benchmark_pairs_" + str(max_files) + ".txt"


threshold = 0.1

with open(filename, 'w') as f:
    files = [os.path.join(folder, file) for file in os.listdir(folder)]
    all_files = files[:max_files]
    similarities = compare_all_files(all_files, LSH_k)
    max_similarity = max(similarities.keys())

    for similarity, files in similarities.items():
        if similarity > threshold:
            for file1, file2 in files:
                f.write(file1[3:])
                f.write('\n')
                f.write(file2[3:])
                f.write('\n\n')

