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

def print_diff_pairs(set1, set2):
    set1_frozen = {frozenset(pair) for pair in set1}
    set2_frozen = {frozenset(pair) for pair in set2}

    diff = set1_frozen - set2_frozen 
    print(len(diff))

    for f1, f2 in diff:
        print(f1 + " " + f2)

lsh_pair_file = f"../{sys.argv[1]}"
benchmarking_folder = '../outputs/'
benchmarking_file_addon = sys.argv[2]
lsh_K = int(sys.argv[3])
lsh_H = int(sys.argv[4])
lsh_P = int(sys.argv[5])
num_possible_combos = int(sys.argv[6])

if len(benchmarking_file_addon) > 0:
    benchmarking_file_prefix = 'benchmark_pairs' + '_' + benchmarking_file_addon + '_'
else:
    benchmarking_file_prefix = 'benchmark_pairs'

max_recall = 80 
max_precision = 5

lsh_pairs = read_pairs_file(lsh_pair_file)
# plagiarized_ratio_lsh = 100 * (len(lsh_pairs)/num_possible_combos)

for dirpath, dirnames, filenames in os.walk(benchmarking_folder):

    # if plagiarized_ratio_lsh < 10:
        for filename in filenames:
            # if benchmarking_file_prefix in filename and str(lsh_K) in filename:
            if benchmarking_file_prefix in filename:
                file_path = os.path.join(dirpath, filename)

                benchmarking_pairs = read_pairs_file(file_path)

                precision = compute_overlap_percent(lsh_pairs, benchmarking_pairs)
                recall = compute_overlap_percent(benchmarking_pairs, lsh_pairs)

                
                # plagiarized_ratio_benchmarking = 100 * (len(benchmarking_pairs)/num_possible_combos)

                # ratio = 0
                # if plagiarized_ratio_benchmarking > 0:
                #     ratio = 100 * (plagiarized_ratio_lsh / plagiarized_ratio_benchmarking)

                print_all = False

                if print_all or (recall > 80 and precision > 20):
                    print("K:                       " + str(lsh_K))
                    print("P:                       " + str(lsh_P))
                    print("H:                       " + str(lsh_H))
                    print("Num benchmarking pairs:  " + str(len(benchmarking_pairs)))
                    print("Num LSH pairs:           " + str(len(lsh_pairs)))
                    print(f"Recall:                  {round(recall, 3)}%")
                    print(f"Precision:               {round(precision, 3)}%")
                    print('\n')

