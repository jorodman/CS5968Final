#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <vector>
#include <functional>
#include <set>
#include <unordered_map>
#include <bits/stdc++.h>
#include <cctype>
#include <string>
#include <cctype>
#include <clocale>
#include <cstdlib>
#include <dirent.h>
#include <sstream>
#include <chrono>

#include "c_utils/hashutil.h"

using namespace std;

// making typedef for short declaration
typedef unordered_multimap<uint64_t, string> mm;
typedef mm::iterator mm_it;

uint64_t Hash(string k_gram, int function_number)
{
    const char * key = k_gram.c_str();
    return MurmurHash64A(key, sizeof(key), function_number); 
}

uint64_t HashVec(vector<uint64_t> val)
{
    const void * key = (void *)&val[0];
    return MurmurHash64A(key, sizeof(key), 874); 
}

// Used ChatGPT to generate this function to read files
vector<string> get_files_in_dir(string directory_path)
{
    vector<string> files;
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(directory_path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                files.push_back(directory_path + "/" + ent->d_name);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Could not open directory: " << directory_path << std::endl;
    }

    return files;
}

std::string read_file_contents(string file_path) {
    try {
        std::ifstream file(file_path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        // Handle the error appropriately...
        return "";
    }
}


class PlagiarismDetection {

	private:
        // Maps doc id to the documents k-grams
        map<string, set<string>> document_k_grams;

        // Maps doc id to the min hash of the document for each hash function
        map<string, vector<uint64_t>> minHashes;

        /*
            Maps doc id to list of each document's partitions. A partition is stored as a vector
            document1 -> [[211, 134], [211, 411], [754, 111]]
        */
        map<string, vector<vector<uint64_t>>> partitions;

        // Maps a partition value to document ids
        unordered_multimap<uint64_t, string> hash_table;

        // doc id to list of doc ids
        unordered_multimap<string, unordered_set<string>> similar_docs;

        chrono::high_resolution_clock::time_point m_startTime;
        chrono::high_resolution_clock::time_point m_endTime;

        int K;
        int num_hash_functions;
        int partition_length;
        string directory;

    public:

        void start() {
            m_startTime = std::chrono::high_resolution_clock::now();
        }

        void stop() {
            m_endTime = std::chrono::high_resolution_clock::now();
        }

        void printElapsedTime() {
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(m_endTime - m_startTime);
            double seconds = static_cast<double>(elapsed.count()) / 1000000.0;
            std::cout << "Elapsed time: " << elapsed.count() << " microseconds." << std::endl;
            std::cout << "Elapsed time: " << seconds << " seconds." << std::endl;
        }

        void parse_data(vector<string> file_names){
            
            for(string filename : file_names)
            {
                string contents = read_file_contents(filename);
                set<string> k_grams;
                for (int i = 0; i < (int) contents.length() - this->K; i += this->K){
                    string sub = contents.substr(i, this->K);
                    transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
                    k_grams.insert(sub);
                }
                document_k_grams[filename] = k_grams; 
            }
        }

        void min_hash()
        {
            for(auto entry : document_k_grams)
            {
                string document_id = entry.first;
                set<string> k_grams = entry.second;

                vector<uint64_t> sketch;

                for(int hash_function = 0; hash_function < this->num_hash_functions; hash_function++)
                {
                    uint64_t min = UINT64_MAX;

                    for(string k_gram : k_grams)
                    {
                        uint64_t val = Hash(k_gram, hash_function);
                        if(val < min)
                        {
                            min = val;
                        }
                    }

                    sketch.push_back(min);
                }

                minHashes[document_id] = sketch;
            }
        }

        map<string, vector<vector<uint64_t>>> partition()
        {
            // {1 -> [1, 2, 3, 4, 5, 6]}
            // {1 -> [[1,2], [3, 4]]}
            for (auto entry : minHashes){
                string doc_id = entry.first;
                // std::cout << doc_id << ": ";
                vector<uint64_t> doc_hashes = entry.second;
                vector<vector<uint64_t>> doc_partitions;
                for (int i = 0; i < num_hash_functions; i += partition_length){
                    vector<uint64_t> partition;
                    int count = 0;
                    for (int j = i; count < partition_length; j += 1){
                        partition.push_back(doc_hashes[j]);
                        count += 1;
                    }
                    doc_partitions.push_back(partition);
                }
                partitions[doc_id] = doc_partitions;
            }
            return partitions;
        }
        
        void hash_the_sketches()
        {
            for(auto entry : this->partitions)
            {
                string doc_id = entry.first;
                vector<vector<uint64_t>> partitions = entry.second;
                   
                for(vector<uint64_t> partitionValues : partitions)
                {
                    uint64_t key = HashVec(partitionValues);

                    hash_table.insert(make_pair(key, doc_id));
                }
            }
        }

        void find_collisions()
        {
            unordered_set<uint64_t> visited;
            unordered_set<string> docs_to_test;
            
            // Open the file to write to
            ofstream outfile("output2.txt");
        
            for (mm_it it = hash_table.begin(); it != hash_table.end(); it++){
                uint64_t key = it->first;
                
                if(visited.count(key) < 1)
                {
                    pair<mm_it, mm_it> range = hash_table.equal_range(key);
                    size_t range_size = distance(range.first, range.second);

                    if(range_size > 1)
                    {
                        // cout << key << ": ";
                        for (auto pair = range.first; pair != range.second; ++pair) {
                            string doc_name = pair->second;
                            if(docs_to_test.count(doc_name) < 1)
                                outfile << doc_name << endl;
                            docs_to_test.insert(doc_name);
                            // cout << (format_doc_name(doc_name)) << " ";
                        }
                        // cout << endl;              
                    }
                    visited.insert(key);
                }
            }


            outfile.close();
        }

        string format_doc_name(string str)
        {
            size_t pos = str.find("documents/");

            // If "documents/" is found, erase it and everything before it
            if (pos != string::npos) {
                str.erase(0, pos + 10);
            }

            return str;
        }

        void print_sketches()
        {
            cout << "Printing document sketches (each row contains min hash of that document for each hash function)" << endl;
            for(auto entry : minHashes)
            {
                cout << entry.first << ": ";

                for(uint64_t val : entry.second)
                {
                    cout << val << " ";
                }

                cout << endl;
            }
        }

        void print_k_grams()
        {
            for (auto entry : document_k_grams){
                cout << entry.first << ": ";

                for(string k_gram : entry.second)
                {
                    cout << k_gram << ",";
                }
                
                cout << endl << endl;
            }
        }

        void print_partitions()
        {
            for (auto entry : partitions){
                string doc_id = entry.first;
                cout << doc_id << ": ";
                vector<vector<uint64_t>> doc_partitions = entry.second;
                for (vector<uint64_t> part : doc_partitions){
                    cout << "[";
                    for(int i = 0; i < (int) part.size(); i++){
                        cout << part[i];

                        if(i < (int) part.size() - 1) cout << ", ";
                    }
                    cout << "], ";
                }
                cout << endl << endl;
            }
        }

        PlagiarismDetection(int K, int num_hash_functions, int partition_length, string directory)
        {
            this->K = K;
            this->num_hash_functions = num_hash_functions;
            this->partition_length = partition_length;
            this->directory = directory;
        }
};


int main(int argc, char *argv[]){
    int chars_per_k_gram = 4;
    int num_hash_functions = 10;
    int partition_length = 2;

    string document_folder = "documents";

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

    PlagiarismDetection * pd = new PlagiarismDetection(chars_per_k_gram, num_hash_functions, partition_length, document_folder);


    pd->start();

    vector<string> file_names = get_files_in_dir(document_folder);
    pd->parse_data(file_names);

    // pd->print_k_grams();

    pd->min_hash();

    // pd->print_sketches();

    pd->partition();

    // pd->print_partitions();

    pd->hash_the_sketches();
    pd->find_collisions();

    pd->stop();

    pd->printElapsedTime();
    
    return 0;
}