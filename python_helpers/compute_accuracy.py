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

def compute_overlap_percent(set1, set2, set1_name):
    set1_frozen = {frozenset(pair) for pair in set1}
    set2_frozen = {frozenset(pair) for pair in set2}
    intersection = {tuple(pair) for pair in (set1_frozen & set2_frozen)}
    # print('\n')
    # print(set1)
    # print(set2)
    # print("intersection: " + str(intersection))


    if len(set1) == 0 and len(set2) == 0:
        overlap_percent = 100
    elif len(set1) == 0:
        overlap_percent = 0
    elif len(set2) == 0:
        overlap_percent = 0
    else:
        overlap_percent = len(intersection) / len(set1) * 100

    return overlap_percent

lsh_pair_file = f"../{sys.argv[1]}"
benchmarking_folder = '../outputs/'
benchmarking_file_addon = sys.argv[2]
num_possible_combos = int(sys.argv[3])
print('Num possible pairs: ' + str(num_possible_combos))

if len(benchmarking_file_addon) > 0:
    benchmarking_file_prefix = 'benchmark_pairs' + '_' + benchmarking_file_addon + '_'
else:
    benchmarking_file_prefix = 'benchmark_pairs'

# print('LSH:                       ' + lsh_pair_file)
# print("Benchmarking files prefix: " + benchmarking_file_prefix)
# print('\n')

# print("Precision: Percentage of detected documents that are actually plagiarized")
# print("Recall: Percentage of actually plagiarized docs that we detected")

for dirpath, dirnames, filenames in os.walk(benchmarking_folder):
    for filename in filenames:
        if benchmarking_file_prefix in filename:
            file_path = os.path.join(dirpath, filename)

            lsh_pairs = read_pairs_file(lsh_pair_file)
            benchmarking_pairs = read_pairs_file(file_path)

            jaccard = filename[-7:-4]

            precision = compute_overlap_percent(lsh_pairs, benchmarking_pairs, "lsh")
            recall = compute_overlap_percent(benchmarking_pairs, lsh_pairs, "benchmarking")
            
            plagiarized_ratio_lsh = 100 * (len(lsh_pairs)/num_possible_combos)
            plagiarized_ratio_benchmarking = 100 * (len(benchmarking_pairs)/num_possible_combos)
            ratio = 100 * (plagiarized_ratio_lsh / plagiarized_ratio_benchmarking)

            print("Jaccard:                " + str(jaccard))
            # print("Num LSH pairs:          " + str(len(lsh_pairs)))
            print("Num benchmarking pairs: " + str(len(benchmarking_pairs)))
            # print(f"Precision:              {round(precision, 5)}%")
            print(f"Recall:                 {round(recall)}%")
            # print(f"Plagiarized % Bench:    {round(plagiarized_ratio_benchmarking, 3)}%")
            print(f"Plagiarized % LSH:      {round(plagiarized_ratio_lsh, 3)}%")
            # print(f"Ratio:                  {round(ratio)}%")
            print('\n')
