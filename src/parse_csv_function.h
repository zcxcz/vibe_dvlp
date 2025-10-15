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
struct RegisterInfo {
    int reg_bit_width;
    int reg_value_min;
    int reg_value_max;
    int reg_initial_value;
};

struct RegisterSection {
    map<string, RegisterInfo> reg_map;
    
    void print_values() const;
};

// csv parser function
extern void PrintCSVLine(const vector<string>& line_data);
extern void PrintCSVFile();
extern void LoadCSVFile();
