#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <functional>
#include <set>
#include <bits/stdc++.h>
#include "hashutil.h"

using namespace std;

set<string> parse_data(string filename){
    set<string> k_grams;
    fstream file; 
    file.open(filename);
    string line;
    while(getline(file, line)){
        for (int i = 0; i < line.length() - 3; i += 3){
            k_grams.insert(line.substr(i, 3));
        }
    }
    file.close();
    return k_grams;
}

int main(){
    set<string> k_grams = parse_data("temp_data.txt");
    for (string s : k_grams){
        cout << s << endl;
    }
    return 0;
}