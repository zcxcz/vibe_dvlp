#include "json.hpp"
#include "hls_crop.h"
#include "hls_dpc.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <string>

using json = nlohmann::json;
using namespace std;
using namespace hls;

// 工具函数：从txt文件读取数据到vector（忽略坐标标签）
vector<uint16_t> read_data_to_vector(const string& filename) {
    ifstream input_file(filename);
    vector<uint16_t> data;
    
    if (!input_file) {
        cerr << "Cannot open input file: "<< filename << endl;
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
        
        // 解析数值（支持空格分隔的多通道数据，支持16进制格式）
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
        output_file << setw(4) << setfill('0') << hex << static_cast<int>(data[i]) << "  # (" << row << "," << col << ")\n";
    }
    output_file.close();
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

// 工具函数：将vector数据转换为HLS流
void vector_to_stream(const vector<uint16_t>& data, hls::stream<axis_pixel_t>& stream) {
    for (size_t i = 0; i < data.size(); ++i) {
        axis_pixel_t data_pkt;
        if (data[i] >= 256) {
            std::cerr << "Warning: Value " << data[i] << " exceeds 8-bit range, truncating" << std::endl;
            data_pkt.data = 255; // 8-bit max
        } else {
            data_pkt.data = static_cast<ap_uint<8>>(data[i]);
        }
        data_pkt.last = (i == data.size() - 1) ? 1 : 0;
        stream.write(data_pkt);
    }
}

// 工具函数：将HLS流转换为vector
vector<uint16_t> stream_to_vector(hls::stream<axis_pixel_t>& stream) {
    vector<uint16_t> data;
    while (!stream.empty()) {
        auto data_pkt = stream.read();
        data.push_back(static_cast<uint16_t>(data_pkt.data));
    }
    return data;
}

// HLS Top模块配置结构体
struct HlsTopConfig {
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
HlsTopConfig load_config(const string& config_path) {
    HlsTopConfig config;
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
    config.crop_output_file = data["common"]["hls_crop_output_file"];
    config.dpc_output_file = data["common"]["hls_dpc_output_file"];
    
    return config;
}

int main(int argc, char* argv[]) {
    // 加载配置
    string config_path = "hls_top_config.json";
    if (argc > 1) {
        config_path = argv[1];
    }
    HlsTopConfig config = load_config(config_path);
    
    // 打印配置信息
    cout << "HLS Top Module Configuration:" << endl;
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
    if ( config.generate_random_image == 1) {
        input_image = generate_random_image(config.width, config.height);
        cout << "Random image generated with width: " << config.width << ", height: " << config.height << endl;
        write_vector_to_file(config.random_image_path, input_image, config.width, config.height);
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
    
    if (input_image.empty()) {
        cerr << "Error: Input image is empty" << endl;
        return 1;
    }
    
    // 准备HLS流和寄存器
    hls::stream<axis_pixel_t> input_stream;
    hls::stream<axis_pixel_t> crop_output_stream;
    hls::stream<axis_pixel_t> dpc_input_stream;
    hls::stream<axis_pixel_t> dpc_output_stream;
    
    // 将输入数据转换为HLS流
    vector_to_stream(input_image, input_stream);
    
    // 实例化HLS模块
    HlsCrop hls_crop;
    HlsDpc hls_dpc;

    // HLS裁剪处理
    vector<uint16_t> crop_result;
    if (config.crop_enable) {
        cout << "Running HLS crop module..." << endl;
        HlsCropRegisterInfo crop_regs;
        crop_regs.image_width = config.width;
        crop_regs.image_height = config.height;
        crop_regs.crop_start_x = config.crop_start_x;
        crop_regs.crop_start_y = config.crop_start_y;
        crop_regs.crop_end_x = config.crop_end_x;
        crop_regs.crop_end_y = config.crop_end_y;
        crop_regs.crop_enable = config.crop_enable ? 1 : 0;
        
        hls_crop.process(input_stream, crop_output_stream, crop_regs);
        crop_result = stream_to_vector(crop_output_stream);
        write_vector_to_file(config.crop_output_file, crop_result);
        cout << "HLS crop result saved to: " << config.crop_output_file << endl;
    } else {
        // 不裁剪时，直接将输入流转换为结果
        crop_result = input_image;
    }
    
    // HLS DPC处理
    vector<uint16_t> dpc_result;
    if (config.dpc_enable) {
        cout << "Running HLS DPC module..." << endl;
        // 准备DPC输入流
        vector_to_stream(crop_result, dpc_input_stream);
        
        // 配置DPC寄存器
        HlsDpcRegisterInfo dpc_regs;
        int dpc_width = config.crop_enable ? (config.crop_end_x - config.crop_start_x + 1) : config.width;
        int dpc_height = config.crop_enable ? (config.crop_end_y - config.crop_start_y + 1) : config.height;
        dpc_regs.image_width = dpc_width;
        dpc_regs.image_height = dpc_height;
        dpc_regs.dpc_enable = config.dpc_enable ? 1 : 0;
        dpc_regs.dpc_threshold = config.dpc_threshold;
        
        hls_dpc.Process(dpc_input_stream, dpc_output_stream, dpc_regs);
        dpc_result = stream_to_vector(dpc_output_stream);
        write_vector_to_file(config.dpc_output_file, dpc_result);
        cout << "HLS DPC result saved to: " << config.dpc_output_file << endl;
    }
    
    cout << "HLS top module completed successfully" << endl;
    return 0;
}