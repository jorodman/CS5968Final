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

using namespace std;

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


void write_out_docs_to_test(unordered_set<string> docs)
{
    ofstream outfile("output.txt");

    for(string doc_name : docs)
    {
        outfile << doc_name << endl;
    }

    outfile.close();
}

