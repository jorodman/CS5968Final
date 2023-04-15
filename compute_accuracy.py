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
    print(f"{overlap_percent:.2f}% of pairs in {set1_name} also appear in the other set")
    return overlap_percent

if __name__ == '__main__':
    lsh_file = 'pairs.txt'
    benchmarking_file = 'benchmark_pairs.txt'

    # lsh_file = 'test1.txt'
    # benchmarking_file = 'test2.txt'

    lsh_pairs = read_pairs_file(lsh_file)
    benchmarking_pairs = read_pairs_file(benchmarking_file)

    precision = compute_overlap_percent(lsh_pairs, benchmarking_pairs, "lsh")
    recall = compute_overlap_percent(benchmarking_pairs, lsh_pairs, "benchmarking")

    print(f"Precision: {precision}%")
    print(f"Recall: {recall}%")
