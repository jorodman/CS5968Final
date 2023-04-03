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

void print_sketches(map<string, vector<uint64_t>> min_hashes);
void print_k_grams(map<string, set<string>> document_k_grams);
void print_partitions(map<string, vector<vector<uint64_t>>> partitions);
void write_out_docs_to_test(unordered_set<string> docs);
