#include "json.hpp"
#include "alg_crop.h"
#include "alg_dpc.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <unistd.h>

using json = nlohmann::json;
using namespace std;

// 工具函数：从txt文件读取数据到vector
vector<uint16_t> read_data_to_vector(const string& filename) {
    ifstream input_file(filename);
    vector<uint16_t> data;
    
    if (!input_file) {
        cerr << "Cannot open input file: " << filename << endl;
        return data;
    }
    
    int value;
    while (input_file >> value) {
        data.push_back(static_cast<uint16_t>(value));
    }
    input_file.close();
    return data;
}

// 工具函数：从vector写入数据到txt文件
bool write_vector_to_file(const string& filename, const vector<uint16_t>& data) {
    // 检查文件是否存在
    bool file_exists = (access(filename.c_str(), F_OK) == 0);
    
    if (file_exists) {
        cout << "Output file exists: " << filename << ", trying to open..." << endl;
    } else {
        cout << "Output file does not exist: " << filename << ", preparing to create..." << endl;
        
        // 确保目录存在
        string dir = ".";
        size_t last_slash = filename.find_last_of("/");
        if (last_slash != string::npos) {
            dir = filename.substr(0, last_slash);
            cout << "Creating directory path: " << dir << endl;
            
            // 创建目录（如果不存在）
            #ifdef _WIN32
                system( ("mkdir " + dir).c_str() );
            #else
                system( ("mkdir -p " + dir).c_str() );
            #endif
        }
    }
    
    // 打开文件，确保使用正确的模式创建文件
    ofstream output_file(filename, ios::out | ios::trunc);
    if (!output_file) {
        cerr << "Failed to open output file: " << filename << endl;
        cerr << "Check directory permissions and available space" << endl;
        return false;
    }
    
    cout << "Successfully opened output file: " << filename << endl;
    
    for (const auto& value : data) {
        output_file << static_cast<int>(value) << "\n";
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

int main(int argc, char* argv[]) {
    // 加载配置
    string config_path = "alg_top_config.json";
    if (argc > 1) {
        config_path = argv[1];
    }
    AlgTopConfig config = load_config(config_path);
    
    // 打印配置信息
    cout << "Algorithm Top Module Configuration:" << endl;
    cout << "Width: " << config.width << endl;
    cout << "Height: " << config.height << endl;
    cout << "Crop Enable: " << (config.crop_enable ? "true" : "false") << endl;
    if (config.crop_enable) {
        cout << "Crop Region: (" << config.crop_start_x << "," << config.crop_start_y << ") to (" << config.crop_end_x << "," << config.crop_end_y << ")" << endl;
    }
    cout << "DPC Enable: " << (config.dpc_enable ? "true" : "false") << endl;
    if (config.dpc_enable) {
        cout << "DPC Threshold: " << config.dpc_threshold << endl;
    }
    
    // 加载或生成输入图像
    vector<uint16_t> input_image;
    ifstream input_file(config.input_file);
    if (input_file.is_open()) {
        cout << "Loading input image from: " << config.input_file << endl;
        input_image = read_data_to_vector(config.input_file);
    } else {
        cout << "Input file not found, generating random image..." << endl;
        input_image = generate_random_image(config.width, config.height);
        write_vector_to_file(config.input_file, input_image);
        cout << "Random image saved to: " << config.input_file << endl;
    }
    
    if (input_image.empty()) {
        cerr << "Error: Input image is empty" << endl;
        return 1;
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
            true
        );
        write_vector_to_file(config.crop_output_file, crop_result);
        cout << "Crop result saved to: " << config.crop_output_file << endl;
    } else {
        crop_result = input_image;
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
            config.dpc_threshold
        );
        write_vector_to_file(config.dpc_output_file, dpc_result);
        cout << "DPC result saved to: " << config.dpc_output_file << endl;
    }
    
    cout << "Algorithm top module completed successfully" << endl;
    return 0;
}