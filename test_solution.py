import subprocess
import sys
from python_helpers.python_timer import Timer

def make():
    subprocess.run('make')

def parse_cmd_line(args):

    configs = {
        'compute_benchmarking': False,
        'k': '10',
        'num_hash_functions': '10',
        'partition_length': '2',
        'document_folder': 'plagiarized_documents',
        'hash_table_file': 'outputs/hash_table_plagiarized.txt',
        'pair_file': 'outputs/plagiarized_pairs.txt',
        'benchmarking_file_add_on': '',
    }

    for arg in args[1:]:
        if arg == "--b":
            # response = input('Are you sure you want to compute benchmarking? It will take a couple of minutes... (yes or no)\n')
            # if 'yes' in response:
                configs['compute_benchmarking'] = True
            # response = input('Type in the benchmarking file add on (if wanted): \n')
            # configs['benchmarking_file_add_on'] = response
        elif arg.startswith("--k="):
            configs['k'] = arg.split("=")[1]
        elif arg.startswith("--h="):
            configs['num_hash_functions'] = arg.split("=")[1]
        elif arg.startswith("--p="):
            configs['partition_length'] = arg.split("=")[1]
        elif arg.startswith("--d="):
            configs['document_folder'] = arg.split("=")[1]
        elif arg.startswith("--hf="):
            configs['hash_table_file'] = arg.split("=")[1]
        elif arg.startswith("--pf="):
            configs['pairs_file'] = arg.split("=")[1]

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

def compute_accuracy():
    subprocess.run(["python3", "compute_accuracy.py", configs['pair_file']], cwd="python_helpers")


# MAIN
configs = parse_cmd_line(sys.argv)

# make()
run_lsh(configs)

if configs['compute_benchmarking']:
    run_benchmarking(configs)

compute_accuracy()




