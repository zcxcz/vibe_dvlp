#include "json.hpp"
#include "alg_crop.h"
#include "alg_dpc.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <string>
#include <unistd.h>

using json = nlohmann::json;
using namespace std;

// 工具函数：从txt文件读取数据到vector（忽略坐标标签）
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
        while (iss >> hex >> value) {
            data.push_back(static_cast<uint16_t>(value));
        }
    }
    
    input_file.close();
    return data;
}

// 工具函数：从vector写入数据到txt文件（带坐标信息）
bool write_vector_to_file(const string& filename, const vector<uint16_t>& data, int width, int height) {
    
    // 打开文件，确保使用正确的模式创建文件
    ofstream output_file(filename, ios::out | ios::trunc);
    if (!output_file) {
        cerr << "Failed to open output file: " << filename << endl;
        cerr << "Check directory permissions and available space" << endl;
        return false;
    }
    
    cout << "Successfully opened output file: " << filename << endl;
    
    // 计算每行的像素数，如果未提供宽高，则默认单行
    int pixels_per_row = (width > 0 && height > 0) ? width : data.size();
    
    for (size_t i = 0; i < data.size(); ++i) {
        int row = i / pixels_per_row;
        int col = i % pixels_per_row;
        output_file << setw(4) << setfill('0') << hex << static_cast<int>(data[i]) << "  # (" << row << "," << col << ")\n";
    }
    
    // 刷新缓冲区并检查写入是否成功
    output_file.flush();
    if (output_file.fail()) {
        cerr << "Error writing data to file: " << filename << endl;
        output_file.close();
        return false;
    }
    
    output_file.close();
    cout << "Successfully wrote " << data.size() << " values to file: " << filename << endl;
    return true;
}

// 重载函数：兼容旧的调用方式（不带宽高参数）
bool write_vector_to_file(const string& filename, const vector<uint16_t>& data) {
    return write_vector_to_file(filename, data, 0, 0);
}

// 工具函数：生成随机图像
vector<uint16_t> generate_random_image(int width, int height) {
    vector<uint16_t> image(width * height);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 255);
    for (auto& pixel : image) {
        pixel = static_cast<uint16_t>(distrib(gen));
    }
    return image;
}

// 算法Top模块配置结构体
struct AlgTopConfig {
    int width;
    int height;
    int crop_start_x;
    int crop_start_y;
    int crop_end_x;
    int crop_end_y;
    bool crop_enable;
    bool dpc_enable;
    int dpc_threshold;
    int generate_random_image;
    string random_image_path;
    string input_file;
    string crop_output_file;
    string dpc_output_file;
};

// 从JSON文件加载配置
AlgTopConfig load_config(const string& config_path) {
    AlgTopConfig config;
    ifstream f(config_path);
    if (!f.is_open()) {
        cout << "Error: Cannot open config file: " << config_path << endl;
        return config;
    }
    
    cout << "Success: Open config file: " << config_path << endl;
    json data = json::parse(f);
    config.generate_random_image = data["image_info"]["generate_random_image"];
    config.random_image_path = data["image_info"]["random_image_path"];
    config.width = data["register_info"]["reg_image_width"]["reg_initial_value"][0];
    config.height = data["register_info"]["reg_image_height"]["reg_initial_value"][0];
    config.crop_start_x = data["register_info"]["reg_crop_start_x"]["reg_initial_value"][0];
    config.crop_start_y = data["register_info"]["reg_crop_start_y"]["reg_initial_value"][0];
    config.crop_end_x = data["register_info"]["reg_crop_end_x"]["reg_initial_value"][0];
    config.crop_end_y = data["register_info"]["reg_crop_end_y"]["reg_initial_value"][0];
    config.crop_enable = (data["register_info"]["reg_crop_enable"]["reg_initial_value"][0] != 0);
    config.dpc_enable = (data["register_info"]["reg_dpc_enable"]["reg_initial_value"][0] != 0);
    config.dpc_threshold = data["register_info"]["reg_dpc_threshold"]["reg_initial_value"][0];
    config.input_file = data["common"]["input_file"];
    config.crop_output_file = data["common"]["alg_crop_output_file"];
    config.dpc_output_file = data["common"]["alg_dpc_output_file"];
    
    return config;
}

void print_config(const AlgTopConfig& config) {
    cout << "Algorithm Top Module Configuration:" << endl;
    cout << "Width: " << config.width << endl;
    cout << "Height: " << config.height << endl;
    cout << "Crop Enable: " << (config.crop_enable ? "true" : "false") << endl;
    cout << "Crop Start X: " << config.crop_start_x << endl;
    cout << "Crop Start Y: " << config.crop_start_y << endl;
    cout << "Crop End X: " << config.crop_end_x << endl;
    cout << "Crop End Y: " << config.crop_end_y << endl;
    cout << "DPC Enable: " << (config.dpc_enable ? "true" : "false") << endl;
    cout << "DPC Threshold: " << config.dpc_threshold << endl;
}

int main(int argc, char* argv[]) {
    // 加载配置
    string config_path = "alg_top_config.json";
    if (argc > 1) {
        config_path = argv[1];
    }
    AlgTopConfig config = load_config(config_path);
    print_config(config);

    // 加载或生成输入图像
    vector<uint16_t> input_image;
    if (config.generate_random_image == 1) {
        cout << "Loading random image from: " << config.random_image_path << endl;
        input_image = generate_random_image(config.width, config.height);
        write_vector_to_file(config.random_image_path, input_image);
        cout << "Random image saved to: " << config.random_image_path << endl;
    } else {
        ifstream input_file1(config.input_file);
        ifstream input_file2(config.random_image_path);
        if (input_file1.is_open()) {
            cout << "Loading input image from: " << config.input_file << endl;
            input_image = read_data_to_vector(config.input_file);
        } else if (input_file2.is_open()) {
            cout << "Loading random image from: " << config.random_image_path << endl;
            input_image = read_data_to_vector(config.random_image_path);
        } else {
            cout << "generate random image disabled, Input file not found, random image file not found, exit..." << endl;
            return 1;
        }
    }
    
    // 算法处理
    vector<uint16_t> crop_result;
    if (config.crop_enable) {
        cout << "Running crop algorithm..." << endl;
        AlgCrop crop_alg;
        crop_alg.crop_image(
            input_image, crop_result,
            config.width, config.height,
            config.crop_start_x, config.crop_start_y,
            config.crop_end_x, config.crop_end_y,
            config.crop_enable
        );
        write_vector_to_file(config.crop_output_file, crop_result);
        cout << "Crop enabled, result saved to: " << config.crop_output_file << endl;
    } else {
        crop_result = input_image;
        write_vector_to_file(config.crop_output_file, crop_result);
        cout << "Crop disabled, result saved to: " << config.crop_output_file << endl;
    }
    
    vector<uint16_t> dpc_result;
    if (config.dpc_enable) {
        cout << "Running DPC algorithm..." << endl;
        int dpc_width = config.crop_enable ? (config.crop_end_x - config.crop_start_x + 1) : config.width;
        int dpc_height = config.crop_enable ? (config.crop_end_y - config.crop_start_y + 1) : config.height;
        dpc_result = AlgDpc::process_image(
            crop_result,
            dpc_width,  
            dpc_height,
            config.dpc_enable,
            config.dpc_threshold
        );
        write_vector_to_file(config.dpc_output_file, dpc_result);
        cout << "DPC enabled, result saved to: " << config.dpc_output_file << endl;
    } else {
        dpc_result = crop_result;
        write_vector_to_file(config.dpc_output_file, dpc_result);
        cout << "DPC disabled, result saved to: " << config.dpc_output_file << endl;
    }
    
    cout << "Algorithm top module completed successfully" << endl;
    return 0;
}