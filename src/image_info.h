#ifndef IMAGE_INFO_H
#define IMAGE_INFO_H

#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

struct ImageSection {
    string image_path;
    string image_format;
    int image_data_bitwidth;
    int generate_random_image;
    string random_image_path;
    
    void print_values() const {
        cout << "ImageSection:" << endl;
        cout << "  image_path: " << image_path << endl;
        cout << "  image_format: " << image_format << endl;
        cout << "  image_data_bitwidth: " << image_data_bitwidth << endl;
        cout << "  generate_random_image: " << generate_random_image << endl;
        cout << "  random_image_path: " << random_image_path << endl;
    }
};

struct OutputSection {
    string alg_crop_output_file;
    string alg_dpc_output_file;
    string hls_crop_output_file;
    string hls_dpc_output_file;
    
    void print_values() const {
        cout << "OutputSection:" << endl;
        cout << "  alg_crop_output_file: " << alg_crop_output_file << endl;
        cout << "  alg_dpc_output_file: " << alg_dpc_output_file << endl;
        cout << "  hls_crop_output_file: " << hls_crop_output_file << endl;
        cout << "  hls_dpc_output_file: " << hls_dpc_output_file << endl;
    }
};

struct RegisterSection {
    struct RegisterInfo {
        int reg_bit_width;
        vector<int> reg_initial_value;
        int reg_value_min;
        int reg_value_max;
    };
    
    RegisterInfo reg_image_width;
    RegisterInfo reg_image_height;
    RegisterInfo reg_smooth_filter_enable;
    RegisterInfo reg_smooth_filter_coeff;
    RegisterInfo reg_dpc_enable;
    RegisterInfo reg_dpc_threshold;
    RegisterInfo reg_crop_enable;
    RegisterInfo reg_crop_start_x;
    RegisterInfo reg_crop_start_y;
    RegisterInfo reg_crop_end_x;
    RegisterInfo reg_crop_end_y;
    
    void print_values() const {
        cout << "RegisterSection:" << endl;
        cout << "  reg_image_width: " << reg_image_width.reg_initial_value[0] << endl;
        cout << "  reg_image_height: " << reg_image_height.reg_initial_value[0] << endl;
        cout << "  reg_smooth_filter_enable: " << reg_smooth_filter_enable.reg_initial_value[0] << endl;
        cout << "  reg_smooth_filter_coeff: " << reg_smooth_filter_coeff.reg_initial_value[0] << endl;
        cout << "  reg_dpc_enable: " << reg_dpc_enable.reg_initial_value[0] << endl;
        cout << "  reg_dpc_threshold: " << reg_dpc_threshold.reg_initial_value[0] << endl;
        cout << "  reg_crop_enable: " << reg_crop_enable.reg_initial_value[0] << endl;
        cout << "  reg_crop_start_x: " << reg_crop_start_x.reg_initial_value[0] << endl;
        cout << "  reg_crop_start_y: " << reg_crop_start_y.reg_initial_value[0] << endl;
        cout << "  reg_crop_end_x: " << reg_crop_end_x.reg_initial_value[0] << endl;
        cout << "  reg_crop_end_y: " << reg_crop_end_y.reg_initial_value[0] << endl;
    }
};

// JSON解析函数 - 简化版本
void from_json(const json& j, ImageSection& info) {
    info.image_path = j["image_path"];
    info.image_format = j["image_format"];
    info.image_data_bitwidth = j["image_data_bitwidth"];
    info.generate_random_image = j["generate_random_image"];
    info.random_image_path = j["random_image_path"];
}

void from_json(const json& j, RegisterSection::RegisterInfo& reg) {
    reg.reg_bit_width = j["reg_bit_width"];
    reg.reg_initial_value = j["reg_initial_value"].get<std::vector<int>>();
    reg.reg_value_min = j["reg_value_min"];
    reg.reg_value_max = j["reg_value_max"];
}

void from_json(const json& j, OutputSection& info) {
    info.alg_crop_output_file = j["alg_crop_output_file"];
    info.alg_dpc_output_file = j["alg_dpc_output_file"];
    info.hls_crop_output_file = j["hls_crop_output_file"];
    info.hls_dpc_output_file = j["hls_dpc_output_file"];
}

void from_json(const json& j, RegisterSection& info) {
    from_json(j["reg_image_width"], info.reg_image_width);
    from_json(j["reg_image_height"], info.reg_image_height);
    from_json(j["reg_smooth_filter_enable"], info.reg_smooth_filter_enable);
    from_json(j["reg_smooth_filter_coeff"], info.reg_smooth_filter_coeff);
    from_json(j["reg_dpc_enable"], info.reg_dpc_enable);
    from_json(j["reg_dpc_threshold"], info.reg_dpc_threshold);
    from_json(j["reg_crop_enable"], info.reg_crop_enable);
    from_json(j["reg_crop_start_x"], info.reg_crop_start_x);
    from_json(j["reg_crop_start_y"], info.reg_crop_start_y);
    from_json(j["reg_crop_end_x"], info.reg_crop_end_x);
    from_json(j["reg_crop_end_y"], info.reg_crop_end_y);
}

#endif