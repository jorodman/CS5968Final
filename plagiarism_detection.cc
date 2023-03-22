#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <functional>
#include <set>
#include <bits/stdc++.h>
#include "hashutil.h"

using namespace std;

uint64_t Hash(string k_gram, int function_number)
{
    const char * key = k_gram.c_str();
    return MurmurHash64A(key, sizeof(key), function_number); 
}

class PlagiarismDetection {

	private:
        map<int, set<string>> document_k_grams;
        map<int, vector<int>> minHashes;
        int K;
        string filename;

    public:
        void parse_data(){
            fstream file; 
            file.open(filename);
            string document;
            int document_id = 0;
            while(getline(file, document)){
                set<string> k_grams;
                for (int i = 0; i < document.length() - this->K; i += this->K){
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

                vector<int> sketch;

                for(int hash_function = 0; hash_function < 5; hash_function++)
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

        void print_sketches()
        {
            for(auto entry : minHashes)
            {
                cout << entry.first << ": ";

                for(int val : entry.second)
                {
                    cout << val << " ";
                }

                cout << endl;
            }
        }

        PlagiarismDetection(int K, string filename)
        {
            this->K = K;
            this->filename = filename;
        }
};


int main(){
    PlagiarismDetection * pd = new PlagiarismDetection(3, "temp_data.txt");
    pd->parse_data();
    pd->min_hash();
    pd->print_sketches();
    
    return 0;
}