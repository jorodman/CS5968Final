from itertools import combinations
import sys

hash_table_file = sys.argv[1]
pair_file = sys.argv[2]

master_set = set()

with open(f"../{hash_table_file}", 'r') as f:
    current_set = set()
    for line in f:
        if line.strip():  # If the line is not empty
            current_set.add(line.strip())
        else:
            # We've reached the end of the current set
            for pair in combinations(current_set, 2):
                master_set.add(frozenset(pair))  # Add the pair as a frozen set
            current_set = set()  # Start a new set for the next iteration

with open(f'../{pair_file}', 'w') as f:
    for s in master_set:
        elements = list(s)
        f.write(elements[0] + '\n' + elements[1])
        f.write('\n\n')

