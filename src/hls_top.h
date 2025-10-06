// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <string>

// tool
#include "json.hpp"
#include "print_function.h"
#include "vector_function.hpp"

// ip
#include "hls_info.h"
#include "hls_crop.h"
// #include "hls_dpc.h"

// using
using json = nlohmann::json;
using namespace std;
using namespace hls;

template <typename HLS_INPUT_DATA_TYPE, typename HLS_OUTPUT_DATA_TYPE>
class HlsTop {
public:
    HlsTop();
    ~HlsTop();
    
    // section object
    HlsRegisterSection hls_register_section;
    HlsImageSection hls_image_section;
    HlsOutputSection hls_output_section;

    // data object
    vector<HLS_INPUT_DATA_TYPE> hls_crop_input_image;
    vector<HLS_INPUT_DATA_TYPE> hls_crop_output_image;
    vector<HLS_OUTPUT_DATA_TYPE> hls_dpc_output_image;

    // ip object
    HlsCrop<HLS_INPUT_DATA_TYPE, HLS_INPUT_DATA_TYPE> hls_crop;
    // HlsDpc<HLS_INPUT_DATA_TYPE, HLS_OUTPUT_DATA_TYPE> hls_dpc;
    
// 从JSON文件加载配置
    load_config(const string& config_path) {
        ifstream f(config_path);
        if (!f.is_open()) {
            MAIN_ERROR_1("Error: Cannot open config file");
        }
        
        MAIN_INFO_1("Success Open config file: ";
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

};

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