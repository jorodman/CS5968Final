import os
import sys

def read_pairs_file(filename):
    pairs = set()
    with open(filename, 'r') as f:
        lines = f.readlines()
        for i in range(0, len(lines), 3):
            if(i + 1 < len(lines)):
                pair = (lines[i].strip(), lines[i+1].strip())
                pairs.add(pair)
    return pairs

def compute_overlap_percent(set1, set2):
    set1_frozen = {frozenset(pair) for pair in set1}
    set2_frozen = {frozenset(pair) for pair in set2}
    intersection = {tuple(pair) for pair in (set1_frozen & set2_frozen)}

    if len(set1) == 0 and len(set2) == 0:
        overlap_percent = 100
    elif len(set1) == 0:
        overlap_percent = 0
    elif len(set2) == 0:
        overlap_percent = 0
    else:
        overlap_percent = len(intersection) / len(set1) * 100

    return overlap_percent

lsh_pair_file = "../outputs/pairs.txt"
benchmarking_folder = '../outputs/'
lsh_K = int(sys.argv[1])
lsh_H = int(sys.argv[2])
lsh_P = int(sys.argv[3])

benchmarking_file_prefix = 'benchmark_pairs_'

lsh_pairs = read_pairs_file(lsh_pair_file)

for dirpath, dirnames, filenames in os.walk(benchmarking_folder):

    # if plagiarized_ratio_lsh < 10:
        for filename in filenames:
            # print(filename)
            # if benchmarking_file_prefix in filename and str(lsh_K) in filename:
            if benchmarking_file_prefix in filename:
                file_path = os.path.join(dirpath, filename)

                benchmarking_pairs = read_pairs_file(file_path)

                precision = compute_overlap_percent(lsh_pairs, benchmarking_pairs)
                recall = compute_overlap_percent(benchmarking_pairs, lsh_pairs)

                print_raw = True

                if print_raw:
                    print(str(lsh_K))
                    print(str(lsh_H))
                    print(round(recall))
                    print(round(precision))
                    print('\n')
                else:
                    print("K:                  " + str(lsh_K))
                    print("H:                      " + str(lsh_H))
                    print(f"R:                 {round(recall)}%")
                    print(f"P:              {round(precision)}%")


