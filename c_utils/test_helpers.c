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
#include <algorithm>

using namespace std;
typedef unordered_multimap<uint64_t, string> mm;
typedef mm::iterator mm_it;

void print_sketches(map<string, vector<uint64_t>> min_hashes)
{
    cout << "Printing document sketches (each row contains min hash of that document for each hash function)" << endl;
    for(auto entry : min_hashes)
    {
        cout << entry.first << ": ";

        for(uint64_t val : entry.second)
        {
            cout << val << " ";
        }

        cout << endl;
    }
}

void print_k_grams(map<string, set<string>> document_k_grams)
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

void print_partitions(map<string, vector<vector<uint64_t>>> partitions)
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


void write_hash_table_to_file(unordered_multimap<uint64_t, string> hash_table, string file_name)
{
    ofstream outfile(file_name);
    unordered_set<uint64_t> visited;
    for (mm_it it = hash_table.begin(); it != hash_table.end(); it++){
        uint64_t key = it->first;
        
        if(visited.count(key) < 1)
        {
            pair<mm_it, mm_it> range = hash_table.equal_range(key);
            size_t range_size = distance(range.first, range.second);

            // There is more than one doc in this part of the hash table
            if(range_size > 1)
            {
                for (auto pair = range.first; pair != range.second; ++pair) {
                    string doc_name = pair->second;
                    outfile << doc_name << endl;
                }
                outfile << endl;
            }
            visited.insert(key);
        }
    }
}

