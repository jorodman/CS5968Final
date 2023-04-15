import os
import string
import itertools
import collections
import time

class Timer:
    def start(self):
        self.start_time = time.perf_counter()

    def stop(self):
        self.end_time = time.perf_counter()

    def print_elapsed_time(self):
        elapsed_time = self.end_time - self.start_time
        print("Elapsed time: {:.6f} seconds".format(elapsed_time))


def get_k_grams(text, k):
    words = text.split()
    words = [word.strip(string.punctuation).lower() for word in words]
    k_grams = set()
    for word in words:
        for i in range(len(word) - k + 1):
            k_gram = word[i:i+k]
            k_grams.add(k_gram)
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

def compare_all_files(folder, k):
    files = [os.path.join(folder, file) for file in os.listdir(folder)]
    
    similarities = collections.defaultdict(list)
    for file1, file2 in itertools.combinations(files, 2):
        similarity = compare_files(file1, file2, k)
        similarities[similarity].append((file1, file2))
    return similarities

if __name__ == "__main__":
    # folder = input("Enter the folder path: ")
    # k = int(input("Enter the k value for k-grams: "))

    # TODO run this on the same set of documents as our algorithm - dont use the entire folder
    folder = "documents"
    k = 10
    threshold = 0.1
    step = 0.05
    max_threshold = 0.4

    timer = Timer()
    timer.start()

    similarities = compare_all_files(folder, k)

    prefix = "benchmark_pairs_"
    
    timer.stop()
    timer.print_elapsed_time()


    while threshold <= max_threshold:
        filename = prefix + str(threshold) + ".txt"
        print("Writing to file: " + filename)
        with open(filename, 'w') as f:
            for similarity, files in similarities.items():
                if similarity > threshold:
                    for file1, file2 in files:
                        f.write(file1)
                        f.write('\n')
                        f.write(file2)
                        f.write('\n\n')
        
        threshold += step

