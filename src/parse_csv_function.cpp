// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

// tool
#include "parse_csv_function.h"
#include "print_function.h"

// using
using namespace std;

// register_info struct implementations
void RegisterSection::print_values() const {
    cout << "RegisterSection:" << endl;
    for (auto reg : reg_map) {
        if (reg.second.reg_bit_width == 1) {
            cout << "  " << setw(30) << reg.first << setw(14) << " = " << setw(8) << reg.second.reg_initial_value << endl;
        } else {
            cout << "  " << setw(30) << reg.first << "[" << setw(4) << reg.second.reg_bit_width-1 << ":" << setw(4) << "0" << "] = " << setw(8) << reg.second.reg_initial_value << " " << "(range: " << setw(8) << reg.second.reg_value_min << " ~ " << setw(8) << reg.second.reg_value_max << ")" << endl;
        }
    }
}

void LoadCSVFile(RegisterSection &register_section) {
   string filename = "/home/sheldon/hls_project/vibe_crop/src/register_table.csv";
   ifstream csv_file(filename);
   if (!csv_file.is_open()) {
       cerr << "Error: Could not open the file!" << endl;
   }
   string line;
   vector<string> fields;
   bool first_line = true;
   // Read each line from the CSV file
   while (getline(csv_file, line)) {
       if (first_line) {
           first_line = false;
           continue;
       }
       istringstream line_stream(line);
       string field;
       fields.clear();
    //    MAIN_INFO_1("line: " + line);
       // Split the line into fields using ',' as a delimiter
       while (getline(line_stream, field, ',')) {
           fields.push_back(field);
       }
       RegisterInfo reg_info;
       reg_info.reg_bit_width = stoi(fields[2]);
       reg_info.reg_initial_value = stoi(fields[3]);
       reg_info.reg_value_min = stoi(fields[4]);
       reg_info.reg_value_max = stoi(fields[5]);
       // Print or process the extracted fields
       // PrintCSVLine(fields);
       register_section.reg_map[fields[1]] = reg_info;
   }
   register_section.print_values();
   csv_file.close();
}


// csv parser function
void PrintCSVLine(const vector<string>& line_data) {
   for (size_t i = 0; i < line_data.size(); i++) {
       // 确保完整输出字符串内容，包括0x前缀
       cout << line_data[i];
       // 只在非最后一个元素后添加分隔符
       if (i < line_data.size() - 1) {
           cout << ",";
       }
   }
   cout << endl;
}

void PrintCSVFile() {
   string filename = "/home/sheldon/hls_project/vibe_crop/src/register_table.csv";
   ifstream csv_file(filename);
   if (!csv_file.is_open()) {
       cerr << "Error: Could not open the file!" << endl;
   }
   string line;
   vector<string> fields;
   // Read each line from the CSV file
   while (getline(csv_file, line)) {
       istringstream line_stream(line);
       string field;
       fields.clear();
    //    MAIN_INFO_1("line: " + line);
       // Split the line into fields using ',' as a delimiter
       while (getline(line_stream, field, ',')) {
           fields.push_back(field);
       }
       // Print or process the extracted fields
       PrintCSVLine(fields);
   }
   csv_file.close();
}

int main() {
    // PrintCSVFile();
    RegisterSection rs;
    LoadCSVFile(rs);
    return 0;
}
