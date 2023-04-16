import subprocess
import sys
from python_helpers.python_timer import Timer

def parse_cmd_line(args):

    configs = {
        'compute_benchmarking': False,
        'k': '10',
        'num_hash_functions': '10',
        'partition_length': '2',
    }

    for arg in sys.argv[1:]:
        if arg == "--b":
            configs['compute_benchmarking'] = True
        elif arg.startswith("--k="):
            configs['k'] = arg.split("=")[1]
        elif arg.startswith("--h="):
            configs['num_hash_functions'] = arg.split("=")[1]
        elif arg.startswith("--p="):
            configs['partition_length'] = arg.split("=")[1]

    return configs


def run_lsh(configs):
    print("Running LSH")
    my_timer = Timer()
    my_timer.start()
    subprocess.run(["./test", configs['k'], configs['num_hash_functions'], configs['partition_length']])
    subprocess.run(["python3", "file_conversion.py", configs['k'], configs['num_hash_functions'], configs['partition_length']], cwd="python_helpers")
    my_timer.stop()
    my_timer.print_elapsed_time()

def run_benchmarking(configs):
    print('Running benchmarking')
    benchmarking_timer = Timer()
    benchmarking_timer.start()
    subprocess.run(["python3", "benchmarking.py", configs['k']], cwd="python_helpers")
    benchmarking_timer.stop()
    benchmarking_timer.print_elapsed_time()

def compute_accuracy():
    print('Computing accuracy')
    subprocess.run(["python3", "compute_accuracy.py"], cwd="python_helpers")



# MAIN
configs = parse_cmd_line(sys.argv)

run_lsh(configs)

if configs['compute_benchmarking']: 
    run_benchmarking(configs)

compute_accuracy()




