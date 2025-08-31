#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

// 复制修改后的函数用于测试
vector<uint16_t> read_data_to_vector(const string& filename) {
    ifstream input_file(filename);
    vector<uint16_t> data;
    
    if (!input_file) {
        cerr << "Cannot open input file: " << filename << endl;
        return data;
    }
    
    string line;
    while (getline(input_file, line)) {
        // 跳过空行
        if (line.empty()) continue;
        
        // 查找注释符号#，如果存在则截取前面的部分
        size_t comment_pos = line.find('#');
        if (comment_pos != string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        // 去除前后空格
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 如果处理后为空行，跳过
        if (line.empty()) continue;
        
        // 解析数值（支持空格分隔的多通道数据）
        istringstream iss(line);
        int value;
        while (iss >> value) {
            data.push_back(static_cast<uint16_t>(value));
        }
    }
    
    input_file.close();
    return data;
}

bool write_vector_to_file(const string& filename, const vector<uint16_t>& data, int width, int height) {
    ofstream output_file(filename);
    if (!output_file) {
        cerr << "Cannot open output file: " << filename << endl;
        return false;
    }
    
    // 计算每行的像素数，如果未提供宽高，则默认单行
    int pixels_per_row = (width > 0 && height > 0) ? width : data.size();
    
    for (size_t i = 0; i < data.size(); ++i) {
        int row = i / pixels_per_row;
        int col = i % pixels_per_row;
        output_file << static_cast<int>(data[i]) << "  # (" << row << "," << col << ")\n";
    }
    output_file.close();
    return true;
}

// 重载函数：兼容旧的调用方式（不带宽高参数）
bool write_vector_to_file(const string& filename, const vector<uint16_t>& data) {
    return write_vector_to_file(filename, data, 0, 0);
}

int main() {
    cout << "=== Testing C++ Coordinate Format Functionality ===" << endl;
    
    // Test parameters
    int width = 3;
    int height = 2;
    vector<uint16_t> test_data = {10, 20, 30, 40, 50, 60};
    
    cout << "Test data: ";
    for (auto val : test_data) cout << val << " ";
    cout << endl;
    
    // Test new format with coordinates
    string new_format_file = "test_cpp_with_coordinates.txt";
    write_vector_to_file(new_format_file, test_data, width, height);
    
    cout << "\nFile content (" << new_format_file << "):" << endl;
    ifstream infile(new_format_file);
    string line;
    while (getline(infile, line)) {
        cout << "  " << line << endl;
    }
    infile.close();
    
    // Test reading both formats
    vector<uint16_t> read_data = read_data_to_vector(new_format_file);
    
    cout << "\nRead back data: ";
    for (auto val : read_data) cout << val << " ";
    cout << endl;
    
    // Verify data integrity
    bool match = (test_data.size() == read_data.size());
    for (size_t i = 0; i < test_data.size() && match; ++i) {
        if (test_data[i] != read_data[i]) match = false;
    }
    
    cout << "Data integrity check: " << (match ? "PASSED" : "FAILED") << endl;
    
    // Test old format compatibility
    string old_format_file = "test_cpp_old_format.txt";
    ofstream oldfile(old_format_file);
    for (auto val : test_data) oldfile << val << "\n";
    oldfile.close();
    
    vector<uint16_t> old_read = read_data_to_vector(old_format_file);
    
    cout << "\nOld format compatibility check: ";
    for (auto val : old_read) cout << val << " ";
    cout << endl;
    
    // Clean up
    remove(new_format_file.c_str());
    remove(old_format_file.c_str());
    
    cout << "\n=== C++ Test completed successfully ===" << endl;
    
    return 0;
}