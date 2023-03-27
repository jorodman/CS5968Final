#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <functional>
#include <set>
#include <unordered_map>
#include <bits/stdc++.h>
#include "hashutil.h"

using namespace std;

// making typedef for short declaration
typedef unordered_multimap<uint64_t, int> mm;
typedef mm::iterator mm_it;

uint64_t Hash(string k_gram, int function_number)
{
    const char * key = k_gram.c_str();
    return MurmurHash64A(key, sizeof(key), function_number); 
}

class PlagiarismDetection {

	private:
        // Maps doc id to the documents k-grams
        map<int, set<string>> document_k_grams;

        // Maps doc id to the min hash of the document for each hash function
        map<int, vector<uint64_t>> minHashes;

        /*
            Maps doc id to list of each document's partitions. A partition is stored as a vector
            document1 -> [[211, 134], [211, 411], [754, 111]]
        */
        map<int, vector<vector<uint64_t>>> partitions;

        // Maps a partition value to document ids
        unordered_multimap<uint64_t, int> hash_table;

        // doc id to list of doc ids
        unordered_multimap<int, unordered_set<int>> similar_docs;

        int K;
        int num_hash_functions;
        int partition_length;
        string filename;

    public:
        void parse_data(){
            fstream file; 
            file.open(filename);
            string document;
            int document_id = 0;
            while(getline(file, document)){
                set<string> k_grams;
                for (int i = 0; i < (int) document.length() - this->K; i += this->K){
                    // TODO normalize the documents
                    k_grams.insert(document.substr(i, this->K));
                }
                document_k_grams[document_id] = k_grams; 
                document_id++;
            }
            file.close();
        }

        void min_hash()
        {
            for(auto entry : document_k_grams)
            {
                int document_id = entry.first;
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

        map<int, vector<vector<uint64_t>>> partition()
        {
            // {1 -> [1, 2, 3, 4, 5, 6]}
            // {1 -> [[1,2], [3, 4]]}
            for (auto entry : minHashes){
                int doc_id = entry.first;
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
                int doc_id = entry.first;
                vector<vector<uint64_t>> partitions = entry.second;
                   
                for(vector<uint64_t> partitionValues : partitions)
                {
                    // For now we just use the first partition as the value to be hashed to get the key
                    uint64_t key = partitionValues[0];

                    // if(hash_table.count(key) > 0)
                    // {
                    //     similar_docs
                    // }

                    hash_table.insert(make_pair(key, doc_id));
                }
            }
        }

        void find_collisions()
        {
            unordered_set<uint64_t> visited;
        
            for (mm_it it = hash_table.begin(); it != hash_table.end(); it++){
                uint64_t key = it->first;
                
                if(visited.count(key) < 1)
                {
                    pair<mm_it, mm_it> range = hash_table.equal_range(key);
                    size_t range_size = distance(range.first, range.second);

                    if(range_size > 1)
                    {
                        cout << key << ": ";
                        for (auto pair = range.first; pair != range.second; ++pair) {
                            cout << (pair->second + 1) << " ";
                            // if(similar_docs[doc_id])
                            // similar_docs[doc_id]
                        }
                        cout << endl;              
                    }
                    visited.insert(key);
                }
            }
            
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
                int doc_id = entry.first;
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

        PlagiarismDetection(int K, int num_hash_functions, int partition_length, string filename)
        {
            this->K = K;
            this->num_hash_functions = num_hash_functions;
            this->partition_length = partition_length;
            this->filename = filename;
        }
};


int main(){
    int chars_per_k_gram = 3;
    int num_hash_functions = 6;
    int partition_length = 1;

    PlagiarismDetection * pd = new PlagiarismDetection(chars_per_k_gram, num_hash_functions, partition_length, "temp_data_test.txt");
    pd->parse_data();

    pd->print_k_grams();

    pd->min_hash();

    // pd->print_sketches();

    pd->partition();

    pd->print_partitions();

    pd->hash_the_sketches();
    pd->find_collisions();
    
    return 0;
}