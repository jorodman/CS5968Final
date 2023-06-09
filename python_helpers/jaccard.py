import os
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

def compare_all_files(folder, k, file_filter, max_files_to_test):
    files = [os.path.join(folder, file) for file in os.listdir(folder)]
    files = [x for x in files if file_filter in x]
    files = files[0:max_files_to_test]
    similarities = collections.defaultdict(list)
    for file1, file2 in itertools.combinations(files, 2):
        similarity = compare_files(file1, file2, k)
        similarities[similarity].append((file1, file2))
    return similarities


K = 5
folder = "DOCUMENTS/chatGPT"
threshold = 0.1
max_files_to_test = 400
file_filter = 'txt'

if len(sys.argv) == 2:
    K = int(sys.argv[1])
elif len(sys.argv) == 3:
    K = int(sys.argv[1])
    threshold = float(sys.argv[2])
elif len(sys.argv) == 4:
    K = int(sys.argv[1])
    threshold = float(sys.argv[2])
    folder = f"{sys.argv[3]}"
elif len(sys.argv) == 5:
    K = int(sys.argv[1])
    threshold = float(sys.argv[2])
    folder = f"{sys.argv[3]}"
    max_files_to_test = int(sys.argv[4])
elif len(sys.argv) == 6:
    K = int(sys.argv[1])
    threshold = float(sys.argv[2])
    folder = f"{sys.argv[3]}"
    max_files_to_test = int(sys.argv[4])
    file_filter = f"{sys.argv[5]}"


similarities = compare_all_files(folder, K, file_filter, max_files_to_test)
max_similarity = max(similarities.keys())
min_similarity = min(similarities.keys())

print('K:           ' + str(K))
print('Threshold:   ' + str(threshold))
print('MAX:         ' + str(max_similarity))
print('MIN:         ' + str(min_similarity))

less = 0
greater = 0

for similarity, files in similarities.items():
    if similarity < threshold:
        less += 1
    elif similarity >= threshold:
        greater += 1

num_elements = len(similarities.keys())
percent_less = round(less / num_elements, 5)
percent_greater = round(greater / num_elements, 5)

print('Less:        ' + str(percent_less))
print('Greater:     ' + str(percent_greater))

