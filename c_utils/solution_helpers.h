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

#include "hashutil.h"

using namespace std;

uint64_t Hash(string k_gram, int function_number);
uint64_t HashVec(vector<uint64_t> val);
vector<string> get_files_in_dir(string directory_path);
string read_file_contents(string file_path);
string trim_folder_from_doc_name(string doc_name, string folder);
