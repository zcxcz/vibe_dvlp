#include "json.hpp"
#include "alg_info.h"
#include "alg_crop.h"
#include "alg_dpc.h"
#include "hls_crop.h"
#include "hls_dpc.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <hls_stream.h>
#include <algorithm>
#include <ap_int.h>

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
    ofstream output_file(filename);
    if (!output_file) {
        cerr << "Cannot open output file: " << filename << endl;
        return false;
    }
    
    for (const auto& value : data) {
        output_file << static_cast<int>(value) << "\n";
    }
    output_file.close();
    return true;
}

// 工具函数：比较两个vector是否一致
bool compare_vectors(const vector<uint16_t>& vec1, const vector<uint16_t>& vec2) {
    if (vec1.size() != vec2.size()) {
        cerr << "Vector sizes differ: " << vec1.size() << " vs " << vec2.size() << endl;
        return false;
    }
    
    for (size_t i = 0; i < vec1.size(); ++i) {
        if (vec1[i] != vec2[i]) {
            cerr << "Data mismatch at position " << i << ": " << vec1[i] << " vs " << vec2[i] << endl;
            return false;
        }
    }
    
    return true;
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


int main(const int argc, const char *argv[]) {
    try {
        // 读取JSON配置 - 兼容不同构建目录
    string config_path = "./vibe.json";
    ifstream f(config_path);
    if (!f.is_open()) {
        // 尝试data子目录路径
        config_path = "./data/vibe.json";
        f.open(config_path);
    }
    if (!f.is_open()) {
        // 尝试上级目录的data子目录路径
        config_path = "../data/vibe.json";
        f.open(config_path);
    }
    if (!f.is_open()) {
        cerr << "Error: Cannot open vibe.json configuration file" << endl;
        return 1;
    }

    cout << "vibe.json configuration file path: " << config_path << endl;
    
        json data = json::parse(f);
        
        ImageInfo img_info = data["image_info"].get<ImageInfo>();
        RegisterInfo reg_info = data["register_info"].get<RegisterInfo>();
        
        int width = reg_info.reg_image_width.reg_initial_value[0];
        int height = reg_info.reg_image_height.reg_initial_value[0];

        img_info.print_values();
        reg_info.print_values();
        
        vector<uint16_t> input_image;   

        // 生成或读取输入图像 - 多路径兼容处理
        if (img_info.generate_random_image == 1) {
            cout << "Generating random image using algorithm model..." << endl;
            // 生成随机图像
            input_image.resize(width * height);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, 255);
            for (auto& pixel : input_image) {
                pixel = static_cast<uint16_t>(distrib(gen));
            }
            
            // 保存生成的随机图像到文件
            vector<string> save_paths = {
                // "../data/" + img_info.random_image_path,
                "./data/" + img_info.random_image_path,
                "./" + img_info.random_image_path
            };
            
            bool image_saved = false;
            for (const auto& path : save_paths) {
                cout << "Trying to save random image: " << path << endl;
                if (write_vector_to_file(path, input_image)) {
                    cout << "Successfully saved random image: " << path << endl;
                    image_saved = true;
                    break;
                }
            }
            
            if (!image_saved) {
                cerr << "Failed to save random image to any path" << endl;
                return 1;
            }
            
            // 从文件中加载随机图像
            vector<string> image_paths = {
                // "../data/"+img_info.random_image_path,
                "./data/"+img_info.random_image_path,
                "./"+img_info.random_image_path
            };
            bool image_load = false;
            for (const auto& path : image_paths) {
                cout << "Trying to load random image: " << path << endl;
                input_image = read_data_to_vector(path);
                if (!input_image.empty()) {
                    cout << "Successfully load image: " << path << endl;
                    image_load = true;
                    break;
                }
            }
            
            if (!image_load) {
                cerr << "Failed to load random image from any path" << endl;
                return 1;
            }
        } else {
            // 多路径尝试加载图像文件
            vector<string> image_paths = {
                "../data/" + img_info.image_path,
                "./data/" + img_info.image_path,
                "./" + img_info.image_path
            };
            
            bool image_load = false;
            for (const auto& path : image_paths) {
                cout << "Trying to load image: " << path << endl;
                input_image = read_data_to_vector(path);
                if (!input_image.empty()) {
                    cout << "Successfully load image: " << path << endl;
                    image_load = true;
                    break;
                }
            }
            
            if (!image_load) {
                cerr << "Failed to load input image from any path" << endl;
                return 1;
            }
        }
        
        // 算法模型处理
        cout << "Running algorithm model crop..." << endl;
        // 使用算法模型进行裁剪
            AlgCrop crop_alg;
            std::vector<uint16_t> alg_result;
            crop_alg.crop_image(
                input_image, alg_result,
                width, height,
                reg_info.reg_crop_start_x.reg_initial_value[0],
                reg_info.reg_crop_start_y.reg_initial_value[0],
                reg_info.reg_crop_end_x.reg_initial_value[0],
                reg_info.reg_crop_end_y.reg_initial_value[0],
                reg_info.reg_crop_enable.reg_initial_value[0] != 0
            );
        
        // 将算法模型的结果写入文件 - 优先尝试data子文件夹，不存在则当前目录
        string alg_output_file;
        string data_path = "./data/alg_crop_data_out.txt";
        vector<string> data_paths = {
            // "../data/"+img_info.random_image_path,
            "./data/alg_crop_data_out.txt",
            "./alg_crop_data_out.txt",
        };
        bool alg_data_store = false;
        for (const auto& path : data_paths) {
        // 尝试创建data子文件夹下的文件
            if (write_vector_to_file(path, alg_result)) {
                alg_output_file = path;
                cout << "Algorithm model result written to " << alg_output_file << endl;
                alg_data_store = true;
                break;
            }
        }
        
        if (alg_data_store == false) {
            cerr << "Failed to store algorithm result to data folder" << endl;
            return 1;
        }
        cout << "Algorithm model completed. Output: " << alg_result.size() << " pixels" << endl;
        
        // 算法模型DPC处理
        cout << "Running algorithm model DPC..." << endl;
        int crop_width = reg_info.reg_crop_end_x.reg_initial_value[0] - reg_info.reg_crop_start_x.reg_initial_value[0] + 1;
        int crop_height = reg_info.reg_crop_end_y.reg_initial_value[0] - reg_info.reg_crop_start_y.reg_initial_value[0] + 1;
        
        std::vector<alg_pixel_t> dpc_result = AlgDpc::process_image(
            alg_result, 
            crop_width, 
            crop_height,
            reg_info.reg_dpc_threshold[0]
        );
        
        // 将DPC算法模型的结果写入文件
        string dpc_output_file;
        vector<string> dpc_data_paths = {
            "./data/alg_dpc_data_out.txt",
            "./alg_dpc_data_out.txt",
        };
        bool dpc_data_store = false;
        for (const auto& path : dpc_data_paths) {
            if (write_vector_to_file(path, dpc_result)) {
                dpc_output_file = path;
                cout << "Algorithm DPC model result written to " << dpc_output_file << endl;
                dpc_data_store = true;
                break;
            }
        }
        
        if (dpc_data_store == false) {
            cerr << "Failed to store DPC algorithm result to data folder" << endl;
            return 1;
        }
        cout << "Algorithm DPC model completed. Output: " << dpc_result.size() << " pixels" << endl;
        
        // HLS模型处理
        cout << "Running HLS model crop..." << endl;
        
        // 准备HLS寄存器
        HlsRegisterInfo hls_regs;
        hls_regs.image_width = reg_info.reg_image_width.reg_initial_value[0];
        hls_regs.image_height = reg_info.reg_image_height.reg_initial_value[0];
        hls_regs.crop_start_x = reg_info.reg_crop_start_x.reg_initial_value[0];
        hls_regs.crop_start_y = reg_info.reg_crop_start_y.reg_initial_value[0];
        hls_regs.crop_end_x = reg_info.reg_crop_end_x.reg_initial_value[0];
        hls_regs.crop_end_y = reg_info.reg_crop_end_y.reg_initial_value[0];
        hls_regs.crop_enable = (reg_info.reg_crop_enable.reg_initial_value[0] != 0) ? 1 : 0;
        hls_regs.dpc_enable = (reg_info.reg_dpc_enable.reg_initial_value[0] != 0) ? 1 : 0;
        hls_regs.dpc_threshold = reg_info.reg_dpc_threshold.reg_initial_value[0];

        // 为crop_hls函数准备参数
        HlsCropRegisterInfo crop_regs;
        crop_regs.image_width = hls_regs.image_width;
        crop_regs.image_height = hls_regs.image_height;
        crop_regs.crop_start_x = hls_regs.crop_start_x;
        crop_regs.crop_start_y = hls_regs.crop_start_y;
        crop_regs.crop_end_x = hls_regs.crop_end_x;
        crop_regs.crop_end_y = hls_regs.crop_end_y;
        crop_regs.crop_enable = hls_regs.crop_enable;
        
        // 创建HLS流
        hls::stream<axis_pixel_t> input_stream;
        hls::stream<axis_pixel_t> crop_output_stream;
        
        // 将输入数据转换为HLS流
        vector_to_stream(input_image, input_stream);
        
        // 运行HLS裁剪模型
        crop_hls(input_stream, crop_output_stream, crop_regs);

        // 将裁剪后的流转换为vector，再转换为DPC输入流
        vector<uint16_t> crop_result = stream_to_vector(crop_output_stream);
        hls::stream<axis_pixel_t> dpc_input_stream;
        vector_to_stream(crop_result, dpc_input_stream);

        // 准备HLS DPC寄存器
        HlsDpcRegisterInfo dpc_regs;
        dpc_regs.image_width = reg_info.reg_crop_end_x.reg_initial_value[0] - reg_info.reg_crop_start_x.reg_initial_value[0] + 1;
        dpc_regs.image_height = reg_info.reg_crop_end_y.reg_initial_value[0] - reg_info.reg_crop_start_y.reg_initial_value[0] + 1;
        dpc_regs.dpc_enable = hls_regs.dpc_enable;
        dpc_regs.dpc_threshold = hls_regs.dpc_threshold;

        // 创建额外的流用于DPC处理
        hls::stream<axis_pixel_t> dpc_output_stream;

        // 运行HLS DPC模型
        dpc_hls(dpc_input_stream, dpc_output_stream, dpc_regs);

        // 将HLS DPC输出转换为vector
        vector<uint16_t> hls_result = stream_to_vector(dpc_output_stream);
        
        // 将HLS模型的结果写入文件 - 优先尝试data子文件夹，不存在则当前目录
        string hls_output_file;
        string hls_data_path = "./data/hls_crop_data_out.txt";
        vector<string> hls_data_paths = {
            "./data/hls_crop_data_out.txt",
            "./hls_crop_data_out.txt",
        };
        bool hls_data_store = false;
        // 尝试创建data子文件夹下的文件
        for (const auto& path : hls_data_paths) {
            if (write_vector_to_file(path, hls_result)) {
                hls_output_file = path;
                cout << "HLS model result written to " << hls_output_file << endl;
                break;
            }
        }
        cout << "HLS model completed. Output: " << hls_result.size() << " pixels" << endl;
        
        // 交叉验证：比较算法模型和HLS模型结果
        cout << "Cross-validating algorithm and HLS models..." << endl;
        if (compare_vectors(alg_result, hls_result)) {
            cout << "SUCCESS: Algorithm and HLS models produce identical results!" << endl;
        } else {
            cerr << "ERROR: Algorithm and HLS models produce different results!" << endl;
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