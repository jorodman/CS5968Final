#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <string>
#include <chrono>

// Solution code
#include "plagiarism_detection.cc"
#include "c_utils/test_helpers.h"

using namespace std;

// GLOBALS 
chrono::high_resolution_clock::time_point m_startTime;
chrono::high_resolution_clock::time_point m_endTime;

void start_timer() {
	m_startTime = chrono::high_resolution_clock::now();
}

void stop_timer() {
	m_endTime = chrono::high_resolution_clock::now();
}

void print_elapsed_time() {
	auto elapsed = chrono::duration_cast<chrono::microseconds>(m_endTime - m_startTime);
	double seconds = static_cast<double>(elapsed.count()) / 1000000.0;
	cout << "Elapsed time: " << elapsed.count() << " microseconds." << endl;
	cout << "Elapsed time: " << seconds << " seconds." << endl;
}

int main(int argc, char *argv[]){
    int chars_per_k_gram = 10;
    int num_hash_functions = 10;
    int partition_length = 2;
    string document_folder = "documents";
    string output_file = "output.txt";

    if(argc == 2)
    {
        chars_per_k_gram = atoi(argv[1]);
    }
    else if(argc == 3)
    {
        chars_per_k_gram = atoi(argv[1]);
        num_hash_functions = atoi(argv[2]);
    }
    else if(argc == 4)
    {
        chars_per_k_gram = atoi(argv[1]);
        num_hash_functions = atoi(argv[2]);
        partition_length = atoi(argv[3]);
    }
    else if(argc == 5)
    {
        chars_per_k_gram = atoi(argv[1]);
        num_hash_functions = atoi(argv[2]);
        partition_length = atoi(argv[3]);
        document_folder = argv[4];
    }

    cout << endl;
    cout << "Running plagiarism detection with the following params:" << endl;
    cout << "K gram length:      " << chars_per_k_gram << endl;
    cout << "Num Hash Functions: " << num_hash_functions << endl;
    cout << "Partition Length:   " << partition_length << endl;
    cout << "Folder with docs:   " << document_folder << endl;
    cout << "Outputing file:     " << output_file << endl;
    cout << endl;

    vector<string> file_names = get_files_in_dir(document_folder);

    PlagiarismDetection * pd = new PlagiarismDetection(chars_per_k_gram, num_hash_functions, partition_length, document_folder);
    
    start_timer();

    pd->parse_data(file_names);
    pd->min_hash();
    pd->partition();
    pd->hash_the_sketches();
    pd->find_collisions();

    stop_timer();

    print_elapsed_time();
    
    cout << endl;
    return 0;
}

