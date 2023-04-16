import os

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
    intersection = set1.intersection(set2)
    overlap_percent = len(intersection) / len(set1) * 100
    # print(f"{overlap_percent:.2f}% of pairs in {set1_name} also appear in the other set")
    return overlap_percent

if __name__ == '__main__':
    lsh_file = '../outputs/pairs.txt'
    benchmarking_folder = '../outputs/'
    benchmarking_file_prefix = 'benchmark_pairs'

    print('\n')
    print("Precision: Percentage of detected documents that are actually plagiarized")
    print("Recall: Percentage of actually plagiarized docs that we detected")
    print('\n')

    for dirpath, dirnames, filenames in os.walk(benchmarking_folder):
        for filename in filenames:
            if benchmarking_file_prefix in filename:
                file_path = os.path.join(dirpath, filename)

                lsh_pairs = read_pairs_file(lsh_file)
                benchmarking_pairs = read_pairs_file(file_path)

                print("Num LSH pairs:          " + str(len(lsh_pairs)))
                print("Num benchmarking pairs: " + str(len(benchmarking_pairs)))

                precision = compute_overlap_percent(lsh_pairs, benchmarking_pairs, "lsh")
                recall = compute_overlap_percent(benchmarking_pairs, lsh_pairs, "benchmarking")

                jaccard = float(filename.split("_")[2].split(".")[0] + "." + filename.split("_")[2].split(".")[1])

                print("Jaccarrd:  " + str(jaccard))
                print(f"Precision: {round(precision)}%")
                print(f"Recall:    {round(recall)}%\n")
