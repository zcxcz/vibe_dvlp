// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

// tool
#include "print_function.h"
#include "vector_function.h"

// using
using namespace std;


// register_info struct
struct RegisterSection {
    map<string, int> reg_map;
    
    void print_values() const;
};

// csv parser function
extern void PrintCSVLine(const vector<string>& line_data);
extern void PrintCSVFile();
extern RegisterSection LoadCSVFile(const string& filename);
