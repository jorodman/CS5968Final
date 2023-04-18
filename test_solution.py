import subprocess
import sys
import os

from python_helpers.python_timer import Timer

def clear_output_folder():
    for filename in os.listdir('outputs'):
        file_path = os.path.join('outputs', filename)
        if os.path.isfile(file_path):
            os.unlink(file_path)

def count_files(path):
    file_list = os.listdir(path)
    file_list = [f for f in file_list if os.path.isfile(os.path.join(path, f))]
    return len(file_list)

# def get_possible_combinations(configs):
#     num_files = count_files(configs['document_folder'])
#     return num_files * (num_files - 1)

def parse_cmd_line(args):

    configs = {
        'compute_benchmarking': False,
        'k': '6',
        'num_hash_functions': '10',
        'partition_length': '1',
        'document_folder': 'DOCUMENTS/chatGPT',
        'hash_table_file': 'outputs/hash_table.txt',
        'pair_file': 'outputs/pairs.txt',
        'benchmarking_file_add_on': '',
        'test_params': False
    }

    for arg in args[1:]:
        if arg.startswith("--k="):
            configs['k'] = arg.split("=")[1]
        elif arg.startswith("--h="):
            configs['num_hash_functions'] = arg.split("=")[1]
        elif arg.startswith("--p="):
            configs['partition_length'] = arg.split("=")[1]
        elif arg.startswith("--d="):
            configs['document_folder'] = arg.split("=")[1]
        elif arg.startswith("--b"):
            response = input('Are you sure you want to compute benchmarking? It will take a couple of minutes... (yes or no)\n')
            if 'yes' in response:
                configs['compute_benchmarking'] = True
                clear_output_folder()
        elif arg.startswith("--o="):
            configs['hash_table_file'] = f"outputs/hash_table_{arg.split('=')[1]}.txt"
            configs['pair_file'] = f"outputs/pairs_{arg.split('=')[1]}.txt"
            configs['benchmarking_file_add_on'] = f"{arg.split('=')[1]}"

    return configs


def run_lsh(configs):
    my_timer = Timer("LSH")
    my_timer.start()
    subprocess.run(["./test", configs['k'], configs['num_hash_functions'], configs['partition_length'], configs['document_folder'], configs['hash_table_file']])
    subprocess.run(["python3", "file_conversion.py", configs['hash_table_file'], configs['pair_file']], cwd="python_helpers")
    my_timer.stop()
    my_timer.print_elapsed_time()

def run_benchmarking(configs):
    benchmarking_timer = Timer("Benchmarking")
    benchmarking_timer.start()
    subprocess.run(["python3", "benchmarking.py", configs['k'], configs['document_folder'], configs['benchmarking_file_add_on']], cwd="python_helpers")
    benchmarking_timer.stop()
    benchmarking_timer.print_elapsed_time()

def compute_accuracy(configs):
    subprocess.run(["python3", "compute_accuracy.py", configs['pair_file'], configs['benchmarking_file_add_on'], configs['k'], configs['num_hash_functions'], configs['partition_length'], str(-1)], cwd="python_helpers")

def compute_and_print_accuracy(configs):
    subprocess.run(["python3", "print_accuracy.py", configs['pair_file'], configs['benchmarking_file_add_on'], configs['k'], configs['num_hash_functions'], configs['partition_length'], str(-1)], cwd="python_helpers")

def compute_and_print_efficiency(configs):
    subprocess.run(["python3", "print_efficiency.py", configs['pair_file'], configs['benchmarking_file_add_on'], configs['k'], configs['num_hash_functions'], configs['partition_length'], str(-1)], cwd="python_helpers")

def find_good_params(configs):
    for k in range(50, 100):
        print('K: ' + str(k))
        for h in range(20, 100, 10):
            for p in range(1, 10):
                configs['partition_length'] = str(p)
                configs['k'] = str(k)
                configs['num_hash_functions'] = str(h)
                run_lsh(configs)
                compute_accuracy(configs)

def print_accuracy(configs):
    for k in range(5, 20):
        for h in range(10, 20, 1):
            configs['k'] = str(k)
            configs['num_hash_functions'] = str(h)
            run_lsh(configs)
            compute_and_print_accuracy(configs)

def print_efficiency(configs):
    k = 6
    h = 10

    for N in range(500):
        configs['k'] = str(k)
        configs['num_hash_functions'] = str(h)
        run_lsh(configs)

# MAIN
configs = parse_cmd_line(sys.argv)

if configs['compute_benchmarking']:
    run_benchmarking(configs)

if configs['test_params']:
    find_good_params(configs)

# ACCURACY STUFF
print_accuracy(configs)

#EFFICIENCY
print_efficiency(configs)






