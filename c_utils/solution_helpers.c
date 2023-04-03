#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <sstream>

#include "hashutil.h"

using namespace std;

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
        cerr << "Could not open directory: " << directory_path << endl;
    }

    return files;
}

string read_file_contents(string file_path) {
    try {
        ifstream file(file_path);
        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return "";
    }
}
