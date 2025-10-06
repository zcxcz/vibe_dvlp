#ifndef VECTOR_FUNCTION_H
#define VECTOR_FUNCTION_H

// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

// tool
#include "print_function.h"

// def
#define VECTOR_FUNCTION_SECTION "[vector_function]"

// using
using namespace std;

// 模板函数实现
template <typename T>
vector<T> vector_read_from_file(const string& filename) {
    ifstream input_file(filename);
    vector<T> data;
    
    if (!input_file) {
        MAIN_ERROR_1("Cannot open input file: " + filename);
        return data;
    }
    
    string line;
    while (getline(input_file, line)) {
        if (line.empty()) continue;
        
        size_t comment_pos = line.find('#');
        if (comment_pos != string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) continue;
        
        istringstream iss(line);
        int value;
        while (iss >> value) {
            data.push_back(static_cast<T>(value));
        }
    }
    
    input_file.close();
    return data;
}

template <typename T>
bool vector_write_to_file(const std::string& filename, const std::vector<T>& data) {
    return vector_write_to_file(filename, data, 0, 0);
}

template <typename T>
bool vector_write_to_file(const std::string& filename, const std::vector<T>& data, int width, int height) {
    ofstream output_file(filename);
    if (!output_file) {
        std::cerr << "Cannot open output file: " << filename << std::endl;
        return false;
    }
    
    int pixels_per_row = (width > 0 && height > 0) ? width : data.size();
    
    for (size_t i = 0; i < data.size(); ++i) {
        int row = i / pixels_per_row;
        int col = i % pixels_per_row;
        output_file << setw(4) << setfill('0') << hex << static_cast<int>(data[i]) << "  # (" << setw(4) << setfill(' ') << dec << row << ", " << setw(4) << setfill(' ') << col << ")\n";
    }
    output_file.close();
    return true;
}

template <typename T, typename U>
bool vector_compare(const std::vector<T>& vec1, const std::vector<U>& vec2) {
    if (vec1.size() != vec2.size()) {
        std::cerr << "Vector sizes differ: " << vec1.size() << " vs " << vec2.size() << std::endl;
        return false;
    }
    
    for (size_t i = 0; i < vec1.size(); ++i) {
        if (vec1[i] != vec2[i]) {
            std::cerr << "Data mismatch at position " << i << ": " << vec1[i] << " vs " << vec2[i] << std::endl;
            return false;
        }
    }
    
    return true;
}

#endif