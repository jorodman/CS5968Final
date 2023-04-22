import subprocess
import sys
import os
import csv

from python_helpers.python_timer import Timer

def clear_output_folder():
    for filename in os.listdir('outputs'):
        file_path = os.path.join('outputs', filename)
        if os.path.isfile(file_path):
            os.unlink(file_path)

def parse_cmd_line(args):

    configs = {
        'compute_benchmarking': False,
        'time_test': False,
        'accuracy_test': False,
        'test_params': False,
        'k': '5',
        'h': '10',
        'p': '1',
        'document_folder': 'DOCUMENTS/test_docs',
        'max_files': '10000',
        'test_large_doc_set': False,
    }

    for arg in args[1:]:
        if arg.startswith("--k="):
            configs['k'] = arg.split("=")[1]
        elif arg.startswith("--h="):
            configs['h'] = arg.split("=")[1]
        elif arg.startswith("--p="):
            configs['p'] = arg.split("=")[1]
        elif arg.startswith("--d="):
            configs['document_folder'] = arg.split("=")[1]
        elif arg.startswith("--b"):
            response = input('Are you sure you want to compute benchmarking? It will take a couple of minutes... (yes or no)\n')
            if 'yes' in response:
                configs['compute_benchmarking'] = True
                # clear_output_folder()
        elif arg.startswith("--t"):
            configs['time_test'] = True
        elif arg.startswith("--l"):
            configs['test_large_doc_set'] = True
        elif arg.startswith("--a"):
            configs['accuracy_test'] = True

    return configs


def run_lsh(configs):
    my_timer = Timer("LSH")
    my_timer.start()
    subprocess.run(["./test", configs['k'], configs['h'], configs['p'], configs['document_folder'], configs['max_files']])
    subprocess.run(["python3", "file_conversion.py", 'outputs/hash_table.txt', 'outputs/pairs.txt'], cwd="python_helpers")
    my_timer.stop()
    my_timer.print_elapsed_time()

def run_benchmarking(configs):
    benchmarking_timer = Timer("Benchmarking")
    benchmarking_timer.start()
    subprocess.run(["python3", "benchmarking.py", configs['k'], configs['document_folder'], configs['max_files']], cwd="python_helpers")
    benchmarking_timer.stop()
    benchmarking_timer.print_elapsed_time()

def compute_accuracy(configs):
    subprocess.run(["python3", "compute_accuracy.py", configs['k'], configs['h'], configs['p']], cwd="python_helpers")

def compute_and_print_accuracy(configs):
    result = subprocess.run(["python3", "print_accuracy.py", configs['k'], configs['h'], configs['p']], cwd="python_helpers", capture_output=True, text=True)
    output_list = result.stdout.strip().split('\n')
    return output_list

def compute_and_print_efficiency(configs):
    subprocess.run(["python3", "print_efficiency.py", configs['k'], configs['h'], configs['p']], cwd="python_helpers")

def find_good_params(configs):

    start_k = 20 
    k_step = 5
    end_k = 100 

    start_h = 10
    h_step = 5
    end_h = 30
    
    start_p = 1
    p_step = 1
    end_p = 4

    for k in range(start_k, end_k, k_step):
        print('K: ' + str(k))
        for h in range(start_h, end_h, h_step):
            for p in range(start_p, end_p, p_step):
                configs['partition_length'] = str(p)
                configs['k'] = str(k)
                configs['num_hash_functions'] = str(h)
                run_lsh(configs)
                compute_accuracy(configs)

def print_accuracy(configs):
    print('Printing accuracy code')
    with open('accuracy_results.csv', 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['K', 'H', 'Recall', 'Precision'])
        for h in range(15, 25, 1):
        # for k in range(100, 150):
            # for h in range(15, 25, 1):
            for k in range(125, 175):
                configs['p'] = str(1)
                configs['k'] = str(k)
                configs['h'] = str(h)
                run_lsh(configs)
                output_list = compute_and_print_accuracy(configs)
                writer.writerow(output_list)


def print_efficiency(configs):
    print('Printing efficiency code comparison between benchmarking and LSH')
    k = 6
    start_num_docs = 20 
    end_num_docs = 400
    num_docs_step = 20


    print('Printing num docs')
    for num_docs in range(start_num_docs, end_num_docs, num_docs_step):
        print(str(num_docs))

    print("Printing benchmarking time")
    for num_docs in range(start_num_docs, end_num_docs, num_docs_step):
        configs['k'] = str(k)
        configs['max_files'] = str(num_docs)
        run_benchmarking(configs)

    # Running with values we expect will be closer to what we will actually report with
    lsh_k = 100
    h = 100

    print("Printing LSH time")
    for num_docs in range(start_num_docs, end_num_docs, num_docs_step):
        configs['k'] = str(lsh_k)
        configs['max_files'] = str(num_docs)
        configs['h'] = str(h)
        run_lsh(configs)


def print_efficiency_lsh(configs):
    print('Printing efficiency code for lsh')
    start_num_docs = 50 
    end_num_docs = 10000
    num_docs_step = 50

    # Running with values we expect will be closer to what we will actually report with
    lsh_k = 100
    h = 100

    print('Printing num docs')
    for num_docs in range(start_num_docs, end_num_docs, num_docs_step):
        print(str(num_docs))

    for num_docs in range(start_num_docs, end_num_docs, num_docs_step):
        configs['k'] = str(lsh_k)
        configs['max_files'] = str(num_docs)
        configs['h'] = str(h)
        run_lsh(configs)




# MAIN
configs = parse_cmd_line(sys.argv)

if configs['compute_benchmarking']:
    run_benchmarking(configs)
    run_lsh(configs)

if configs['test_params']:
    find_good_params(configs)

if configs['time_test']:
    print_efficiency(configs)

if configs['test_large_doc_set']:
    print_efficiency_lsh(configs)

if configs['accuracy_test']:
    print_accuracy(configs)







