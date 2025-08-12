#include "json.hpp"
#include "alg_crop.h"
#include "hls_crop.h"
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
        if (data[i] >= 1024) {
            std::cerr << "Warning: Value " << data[i] << " exceeds 10-bit range, truncating" << std::endl;
            data_pkt.data = 1023; // 10-bit max
        } else {
            data_pkt.data = static_cast<pixel_t>(data[i]);
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

// 复用原来的JSON结构定义
struct ImageInfo {
    string image_path;
    string image_format;
    int image_data_bitwidth;
    int generate_random_image;
    string random_image_path;
    
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
    
    int operator[](size_t index) const {
        return index < reg_initial_value.size() ? reg_initial_value[index] : 0;
    }
    
    size_t size() const {
        return reg_initial_value.empty() ? 1 : reg_initial_value.size();
    }
    
    void print_values(const string& name = "Values") const {
        cout << name << ": ";
        if (reg_initial_value.empty()) {
            cout << "0";
        } else if (reg_initial_value.size() == 1) {
            cout << reg_initial_value[0];
        } else {
            for (size_t i = 0; i < reg_initial_value.size(); ++i) {
                cout << reg_initial_value[i];
                if (i < reg_initial_value.size() - 1) cout << " ";
            }
        }
        cout << endl;
    }
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RegInfo, reg_bit_width, reg_initial_value, reg_value_min, reg_value_max)
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
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RegisterInfo, reg_image_width, reg_image_height, reg_filter_coeff, 
                                   reg_crop_start_x, reg_crop_start_y, reg_crop_end_x, reg_crop_end_y, reg_crop_enable)
    
    void print_values() const {
        cout << "=== Register Information ===" << endl;
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
        // 读取JSON配置 - 兼容不同构建目录
        string config_path = "data/vibe.json";
        ifstream f(config_path);
        if (!f.is_open()) {
            // 尝试相对于可执行文件的路径
            config_path = "../data/vibe.json";
            f.open(config_path);
        }
        if (!f.is_open()) {
            // 尝试工作目录路径
            config_path = "./data/vibe.json";
            f.open(config_path);
        }
        if (!f.is_open()) {
            cerr << "Error: Cannot open vibe.json configuration file" << endl;
            return 1;
        }
        json data = json::parse(f);
        
        ImageInfo img_info = data["image_info"].get<ImageInfo>();
        RegisterInfo reg_info = data["register_info"].get<RegisterInfo>();
        
        int width = reg_info.reg_image_width[0];
        int height = reg_info.reg_image_height[0];
        
        img_info.print_values();
        reg_info.print_values();
        
        vector<uint16_t> input_image;
        string image_path = img_info.image_path;
        
        // 生成或读取输入图像
        if (img_info.generate_random_image == 1) {
            cout << "Generating random image using algorithm model..." << endl;
            input_image = AlgCrop::generate_random_image(width, height, 1023);
            image_path = img_info.random_image_path;
            
            if (!AlgCrop::write_image_to_file(image_path, input_image)) {
                cerr << "Failed to save random image" << endl;
                return 1;
            }
            cout << "Random image generated: " << image_path << endl;
        } else {
            cout << "Loading existing image: " << image_path << endl;
            input_image = read_data_to_vector(image_path);
            if (input_image.empty()) {
                cerr << "Failed to load image: " << image_path << endl;
                return 1;
            }
        }
        
        // 算法模型处理
        cout << "Running algorithm model crop..." << endl;
        std::vector<uint16_t> alg_result = AlgCrop::crop_image(
            input_image, width, height,
            reg_info.reg_crop_start_x[0],
            reg_info.reg_crop_start_y[0],
            reg_info.reg_crop_end_x[0],
            reg_info.reg_crop_end_y[0],
            reg_info.reg_crop_enable[0] != 0
        );
        
        if (!AlgCrop::write_image_to_file("./data/alg_crop_data_out.txt", alg_result)) {
            cerr << "Failed to save algorithm model result" << endl;
            return 1;
        }
        cout << "Algorithm model completed. Output: " << alg_result.size() << " pixels" << endl;
        
        // HLS模型处理
        cout << "Running HLS model crop..." << endl;
        
        // 准备HLS寄存器
        RegisterHlsInfo hls_regs;
        hls_regs.image_width = reg_info.reg_image_width[0];
        hls_regs.image_height = reg_info.reg_image_height[0];
        hls_regs.crop_start_x = reg_info.reg_crop_start_x[0];
        hls_regs.crop_start_y = reg_info.reg_crop_start_y[0];
        hls_regs.crop_end_x = reg_info.reg_crop_end_x[0];
        hls_regs.crop_end_y = reg_info.reg_crop_end_y[0];
        hls_regs.crop_enable = (reg_info.reg_crop_enable[0] != 0) ? 1 : 0;
        
        // 创建HLS流
        hls::stream<axis_pixel_t> input_stream;
        hls::stream<axis_pixel_t> output_stream;
        
        // 将输入数据转换为HLS流
        vector_to_stream(input_image, input_stream);
        
        // 运行HLS模型
        crop_hls(input_stream, output_stream, hls_regs);
        
        // 将HLS输出转换为vector
        vector<uint16_t> hls_result = stream_to_vector(output_stream);
        
        if (!write_vector_to_file("./data/hls_crop_data_out.txt", hls_result)) {
            cerr << "Failed to save HLS model result" << endl;
            return 1;
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