#include <cmath>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include <algorithm>

#include "c_utils/solution_helpers.h"

using namespace std;

// making typedef for short declaration
typedef unordered_multimap<uint64_t, string> mm;
typedef mm::iterator mm_it;

struct PairComparator {
    bool operator()(const pair<string, string>& a, const pair<string, string>& b) const {
        if (a == b) {
            return false;
        }
        else if (a.first == b.second && a.second == b.first) {
            return false;
        }
        else {
            return a < b;
        }
    }
};

class PlagiarismDetection {

	private:
        // Maps doc id to the documents k-grams
        map<string, set<string>> documents_k_grams;

        // Maps doc id to the min hash of the document for each hash function
        map<string, vector<uint64_t>> min_hashes;

        /*
            Maps doc id to list of each document's partitions. A partition is stored as a vector
            document1 -> [[211, 134], [211, 411], [754, 111]]
        */
        map<string, vector<vector<uint64_t>>> partitions;

        // Maps a partition value to document ids
        unordered_multimap<uint64_t, string> hash_table;

        // doc id to list of doc ids
        unordered_multimap<string, unordered_set<string>> similar_docs;

        int K;
        int num_hash_functions;
        int partition_length;
        string directory;

    public:

        void make_k_grams(vector<string> file_names){
            
            for(string filename : file_names)
            {
                string contents = read_file_contents(filename);
                set<string> k_grams;
                for (int i = 0; i < (int) contents.length() - this->K; i += this->K){
                    string sub = contents.substr(i, this->K);
                    transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
                    k_grams.insert(sub);
                }
                documents_k_grams[filename] = k_grams; 
            }
        }

        void min_hash()
        {
            for(auto entry : documents_k_grams)
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

                min_hashes[document_id] = sketch;
            }
        }

        map<string, vector<vector<uint64_t>>> partition()
        {
            // {1 -> [1, 2, 3, 4, 5, 6]}
            // {1 -> [[1,2], [3, 4]]}
            for (auto entry : min_hashes){
                string doc_id = entry.first;
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

        unordered_multimap<uint64_t, string> get_hash_table()
        {
            return hash_table;
        }

        // TODO move to helper file or something?
        vector<string> getValues(const unordered_multimap<uint64_t, string>& m, uint64_t key) {
            vector<string> values;
            auto range = m.equal_range(key);
            for (auto it = range.first; it != range.second; ++it) {
                values.push_back(it->second);
            }
            return values;
        }


        map<string, set<string>> getSimilarDocsMap(const unordered_multimap<uint64_t, string>& m)
        {
            map<string, set<string>> similarDocsMap;
            
            for (auto i = m.begin(); i != m.end(); ++i) {
                auto j = i;
                ++j;
                while (j != m.end() && j->first == i->first) {
                    vector<string> values1 = getValues(m, i->first);
                    vector<string> values2 = getValues(m, j->first);
                    for (const auto& value1 : values1) {
                        // set<string> similarDocs;
                        for (const auto& value2 : values2) {
                            if (value1 != value2) {
                                // similarDocsMap[value1].insert(value2);
                                // similarDocsMap.insert(make_pair(value1, value2));
                            }
                        }
                        
                    }
                    ++j;
                }
            }

            return similarDocsMap;
        }
        
        // TODO move to helper file or something?
        set<set<string>> getAllPairs(const unordered_multimap<uint64_t, string>& m) {
            set<set<string>> pairs;

            for (auto i = m.begin(); i != m.end(); ++i) {
                auto j = i;
                ++j;
                while (j != m.end() && j->first == i->first) {
                    vector<string> values1 = getValues(m, i->first);
                    vector<string> values2 = getValues(m, j->first);
                    for (const auto& value1 : values1) {
                        for (const auto& value2 : values2) {
                            if (value1 != value2) {
                                set<string> pair;
                                pair.insert(value1);
                                pair.insert(value2);
                                pairs.insert(pair);
                                // cout << pairs.size() << endl;
                                // string smaller = "";
                                // string greater  = "";
                                // if (value1.compare(value2) < 0){
                                //     smaller = value1;
                                //     greater = value2;
                                // }
                                // else{
                                //     smaller = value2;
                                //     greater = value1;
                                // }
                                // pair<string, string> p = make_pair(smaller, greater);
                                // if (pairs.find(p) != pairs.end()) {
                                //     continue;
                                // }
                                // else{
                                //     pairs.insert(make_pair(smaller, greater));
                                // }           

                                // pairs.insert(make_pair(value1, value2));
                                // pairs.insert(make_pair(value2, value1));
                            }
                        }
                    }
                    ++j;
                }
            }
            return pairs;
        }


        unordered_set<string> find_collisions()
        {
            unordered_set<uint64_t> visited;
            unordered_set<string> docs_to_test;
        
            for (mm_it it = hash_table.begin(); it != hash_table.end(); it++){
                uint64_t key = it->first;
                
                if(visited.count(key) < 1)
                {
                    pair<mm_it, mm_it> range = hash_table.equal_range(key);
                    size_t range_size = distance(range.first, range.second);

                    // There is more than one doc in this part of the hash table
                    if(range_size > 1)
                    {
                        // cout << key << endl;
                        for (auto pair = range.first; pair != range.second; ++pair) {
                            string doc_name = pair->second;
                            // cout << doc_name << endl;
                            docs_to_test.insert(pair->second);
                        }
                        // cout << endl;
                    }
                    visited.insert(key);
                }
            }

            return docs_to_test;
        }


        PlagiarismDetection(int K, int num_hash_functions, int partition_length, string directory)
        {
            this->K = K;
            this->num_hash_functions = num_hash_functions;
            this->partition_length = partition_length;
            this->directory = directory;
        }
};

