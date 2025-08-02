#include "json.hpp"
#include "crop.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <hls_stream.h>
#include <algorithm>

using json = nlohmann::json;
using namespace std;

// 工具函数：从txt文件读取数据到stream并返回数据大小
size_t read_data_to_stream(const string& filename, hls::stream<pixel_t>& stream) {
    ifstream input_file(filename);
    if (!input_file) {
        cerr << "Cannot open input file: " << filename << endl;
        return 0;
    }
    
    size_t count = 0;
    int value;
    while (input_file >> value) {
        stream.write(static_cast<pixel_t>(value));
        count++;
    }
    input_file.close();
    return count;
}

// 工具函数：从stream写入数据到txt文件并返回数据大小
size_t write_stream_to_file(const string& filename, hls::stream<pixel_t>& stream) {
    ofstream output_file(filename);
    if (!output_file) {
        cerr << "Cannot open output file: " << filename << endl;
        return 0;
    }
    
    size_t count = 0;
    while (!stream.empty()) {
        output_file << static_cast<int>(stream.read()) << "\n";
        count++;
    }
    output_file.close();
    return count;
}

// 工具函数：比较两个txt文件的数据是否一致
bool compare_files(const string& file1, const string& file2) {
    ifstream f1(file1);
    ifstream f2(file2);
    
    if (!f1 || !f2) {
        cerr << "Cannot open files for comparison" << endl;
        return false;
    }
    
    vector<int> data1, data2;
    int value;
    
    while (f1 >> value) data1.push_back(value);
    while (f2 >> value) data2.push_back(value);
    
    f1.close();
    f2.close();
    
    if (data1.size() != data2.size()) {
        cerr << "File sizes differ: " << data1.size() << " vs " << data2.size() << endl;
        return false;
    }
    
    for (size_t i = 0; i < data1.size(); ++i) {
        if (data1[i] != data2[i]) {
            cerr << "Data mismatch at position " << i << ": " << data1[i] << " vs " << data2[i] << endl;
            return false;
        }
    }
    
    return true;
}

struct ImageInfo {
    string image_path;
    string image_format;
    int image_data_bitwidth;
    int generate_random_image;
    string random_image_path;
    
    // Print image information
    void print_values() const {
        cout << "=== Image Information ===" << endl;
        cout << "Image Path: " << image_path << endl;
        cout << "Image Format: " << image_format << endl;
        cout << "Image Data Bitwidth: " << image_data_bitwidth << endl;
        cout << "Generate Random Image: " << (generate_random_image ? "Yes" : "No") << endl;
        cout << "Random Image Path: " << random_image_path << endl;
        cout << "========================" << endl;
    }
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ImageInfo, image_path, image_format, image_data_bitwidth, generate_random_image, random_image_path)
};

struct RegInfo {
    int reg_bit_width;
    vector<int> reg_initial_value;
    int reg_value_min;
    int reg_value_max;
    
    // Determine if it's a single value - check by vector size
    bool is_single_value() const {
        return reg_initial_value.size() == 1;
    }
    
    // Overload [] operator to support seamless access
    int operator[](size_t index) const {
        if (reg_initial_value.empty()) {
            return 0;
        }
        return index < reg_initial_value.size() ? reg_initial_value[index] : 0;
    }
    
    // Get the number of values
    size_t size() const {
        return reg_initial_value.empty() ? 1 : reg_initial_value.size();
    }
    
    // Unified printing function, automatically handle single value or array
    void print_values(const string& name = "Values") const {
        cout << name << ": ";
        if (reg_initial_value.empty()) {
            cout << "0";
        } else if (is_single_value()) {
            cout << reg_initial_value[0];
        } else {
            for (size_t i = 0; i < reg_initial_value.size(); ++i) {
                cout << reg_initial_value[i];
                if (i < reg_initial_value.size() - 1) {
                    cout << " ";
                }
            }
        }
        cout << endl;
    }
    
    // Helper functions for JSON serialization
    friend void to_json(json& j, const RegInfo& r) {
        // If there's only one value, serialize as a single integer; otherwise serialize as an array
        if (r.reg_initial_value.size() == 1) {
            j = json{{"reg_bit_width", r.reg_bit_width}, {"reg_initial_value", r.reg_initial_value[0]}, {"reg_value_min", r.reg_value_min}, {"reg_value_max", r.reg_value_max}};
        } else {
            j = json{{"reg_bit_width", r.reg_bit_width}, {"reg_initial_value", r.reg_initial_value}, {"reg_value_min", r.reg_value_min}, {"reg_value_max", r.reg_value_max}};
        }
    }
    
    friend void from_json(const json& j, RegInfo& r) {
        j.at("reg_bit_width").get_to(r.reg_bit_width);
        j.at("reg_value_min").get_to(r.reg_value_min);
        j.at("reg_value_max").get_to(r.reg_value_max);
        
        // Handle reg_initial_value which could be a single integer or an array of integers
        if (j.at("reg_initial_value").is_number()) {
            int single_value;
            j.at("reg_initial_value").get_to(single_value);
            r.reg_initial_value = {single_value};
        } else {
            j.at("reg_initial_value").get_to(r.reg_initial_value);
        }
    }
};

struct RegisterInfo {
    RegInfo reg_image_width;
    RegInfo reg_image_height;
    RegInfo reg_filter_coeff;
    RegInfo reg_crop_start_x;
    RegInfo reg_crop_start_y;
    RegInfo reg_crop_end_x;
    RegInfo reg_crop_end_y;
    RegInfo reg_crop_enable;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RegisterInfo, reg_image_width, reg_image_height, reg_filter_coeff, reg_crop_start_x, reg_crop_start_y, reg_crop_end_x, reg_crop_end_y, reg_crop_enable)
    
    // Print all register information
    void print_values() const {
        cout << "=== Register Information ===" << endl;
        
        // Use helper function to print member information
        print_reg_info("Reg Image Width", reg_image_width);
        print_reg_info("Reg Image Height", reg_image_height);
        print_reg_info("Reg Filter Coeff", reg_filter_coeff);
        print_reg_info("Reg Crop Start X", reg_crop_start_x);
        print_reg_info("Reg Crop Start Y", reg_crop_start_y);
        print_reg_info("Reg Crop End X", reg_crop_end_x);
        print_reg_info("Reg Crop End Y", reg_crop_end_y);
        print_reg_info("Reg Crop Enable", reg_crop_enable);
        
        cout << "==========================" << endl;
    }
    
private:
    // Helper function to print information of a single RegInfo member
    void print_reg_info(const string& name, const RegInfo& reg) const {
        cout << name << ":" << endl;
        cout << "  Bit Width: " << reg.reg_bit_width << endl;
        cout << "  Min Value: " << reg.reg_value_min << endl;
        cout << "  Max Value: " << reg.reg_value_max << endl;
        reg.print_values("  Initial Value");
    }
};

int main(const int argc, const char *argv[]) {
    try {
        // Read JSON file
        ifstream f("./data/vibe.json");
        json data = json::parse(f);
        
        // Directly convert JSON data to struct
        ImageInfo img_info = data["image_info"].get<ImageInfo>();
        RegisterInfo reg_info = data["register_info"].get<RegisterInfo>();
        
        // Get image parameters (using [] operator directly)
        int width = reg_info.reg_image_width[0];
        int height = reg_info.reg_image_height[0];

        string image_path = img_info.image_path;
        string image_format = img_info.image_format;
        
        // Print image and register` information using unified print_values function
        img_info.print_values();
        reg_info.print_values();
        
        // 根据generate_random_image属性决定是否生成随机图像
        if (img_info.generate_random_image == 1) {
            cout << "Generating random image data..." << endl;
            image_path = img_info.random_image_path;
            string command = string("python3 ./py/generate_random_image.py") +
                " --width " + to_string(width) + 
                " --height " + to_string(height) + 
                " --format " + image_format +
                " --output " + image_path;
            
            int result = system(command.c_str());
            if (result == 0) {
                cout << "Random image generated successfully: " << image_path << endl;
            } else {
                cerr << "Failed to generate random image" << endl;
                return 1;
            }
        } else {
            cout << "Using existing image file: " << image_path << endl;
        }
        
        // 使用crop.h中定义的RegisterHlsInfo结构体
        RegisterHlsInfo hls_regs;
        hls_regs.image_width = reg_info.reg_image_width[0];
        hls_regs.image_height = reg_info.reg_image_height[0];
        hls_regs.crop_start_x = reg_info.reg_crop_start_x[0];
        hls_regs.crop_start_y = reg_info.reg_crop_start_y[0];
        hls_regs.crop_end_x = reg_info.reg_crop_end_x[0];
        hls_regs.crop_end_y = reg_info.reg_crop_end_y[0];
        hls_regs.crop_enable = (reg_info.reg_crop_enable[0] != 0) ? ap_uint<1>(1) : ap_uint<1>(0);
        
        // Python版本crop处理（输出重命名为crop_py_data_out.txt）
        cout << "Starting Python crop processing..." << endl;
        int crop_start_x = reg_info.reg_crop_start_x[0];
        int crop_start_y = reg_info.reg_crop_start_y[0];
        int crop_end_x = reg_info.reg_crop_end_x[0];
        int crop_end_y = reg_info.reg_crop_end_y[0];
        int crop_enable = reg_info.reg_crop_enable[0];
        
        string crop_command = string("python3 ./py/crop.py") +
            " " + image_path +
            " ./data/crop_py_data_out.txt" +
            " " + to_string(crop_start_x) +
            " " + to_string(crop_start_y) +
            " " + to_string(crop_end_x) +
            " " + to_string(crop_end_y) +
            " " + (crop_enable ? "true" : "false") +
            " " + to_string(width) +
            " " + to_string(height) +
            " 10" +  // 使用10-bit位宽
            " " + img_info.image_format;
        
        int crop_result = system(crop_command.c_str());
        if (crop_result == 0) {
            cout << "Python crop processing completed successfully" << endl;
            cout << "Processed image saved to: ./data/crop_py_data_out.txt" << endl;
            
            // 统计Python处理的输出数据大小
            ifstream py_output("./data/crop_py_data_out.txt");
            size_t py_count = 0;
            int value;
            while (py_output >> value) py_count++;
            py_output.close();
            cout << "Python processing - Output pixels: " << py_count << endl;
        } else {
            cerr << "Failed to execute Python crop processing" << endl;
            return 1;
        }
        
        // HLS版本crop处理（统一在vibe.cpp中处理文件读写）
        cout << "Starting HLS crop processing..." << endl;
        
        // 使用封装函数处理HLS数据流
        hls::stream<pixel_t> input_stream;
        hls::stream<pixel_t> output_stream;
        
        size_t input_count = read_data_to_stream(image_path, input_stream);
        crop_hls(input_stream, output_stream, hls_regs);
        size_t output_count = write_stream_to_file("./data/crop_hls_data_out.txt", output_stream);
        
        cout << "HLS processing - Input pixels: " << input_count << endl;
        cout << "HLS processing - Output pixels: " << output_count << endl;
        
        cout << "HLS crop processing completed successfully" << endl;
        cout << "Processed image saved to: ./data/crop_hls_data_out.txt" << endl;
        
        // 对比Python和HLS处理结果
        cout << "Comparing Python and HLS processing results..." << endl;
        bool is_match = compare_files("./data/crop_py_data_out.txt", "./data/crop_hls_data_out.txt");
        
        if (is_match) {
            cout << "SUCCESS: Python and HLS processing results match!" << endl;
        } else {
            cerr << "ERROR: Python and HLS processing results do not match!" << endl;
            return 1;
        }
        
    } catch (json::parse_error& e) {
        cerr << "JSON parsing error: " << e.what() << endl;
        return 1;
    } catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
