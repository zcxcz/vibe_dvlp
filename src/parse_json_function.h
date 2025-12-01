#ifndef IMAGE_INFO_H
#define IMAGE_INFO_H

// std
#include <string>
#include <iostream>
#include <fstream>

// tool
#include "json.hpp"
#include "print_function.h"

using json = nlohmann::json;
using namespace std;


struct ImageSection {
    string src_image_format;
    int src_image_data_bitwidth;
    int generate_random_src_image_enable;
    
    void print_values() const {
        cout << "ImageSection:" << endl;
        cout << "  src_image_format: " << src_image_format << endl;
        cout << "  src_image_data_bitwidth: " << src_image_data_bitwidth << endl;
        cout << "  generate_random_src_image_enable: " << generate_random_src_image_enable << endl;
    }
};

struct OutputSection {
    string src_image_path;
    string random_src_image_path;
    string alg_crop_output_path;
    string alg_dpc_output_path;
    string py_dpc_output_path;
    string hls_crop_output_path;
    string hls_dpc_output_path;
    
    void print_values() const {
        cout << "OutputSection:" << endl;
        cout << "  src_image_path: " << src_image_path << endl;
        cout << "  random_src_image_path: " << random_src_image_path << endl;
        cout << "  alg_crop_output_path: " << alg_crop_output_path << endl;
        cout << "  alg_dpc_output_path: " << alg_dpc_output_path << endl;
        cout << "  py_dpc_output_path: " << py_dpc_output_path << endl;
        cout << "  hls_crop_output_path: " << hls_crop_output_path << endl;
        cout << "  hls_dpc_output_path: " << hls_dpc_output_path << endl;
    }
};

/*
struct RegisterInfo {
    int reg_bit_width;
    vector<int> reg_initial_value;
    int reg_value_min;
    int reg_value_max;
};

struct RegisterSection {
    map<string, RegisterInfo> reg_map;
    
    void print_values() const {
        cout << "RegisterSection:" << endl;
        for (auto reg : reg_map) {
            if (reg.second.reg_bit_width == 1) {
                cout << "  " << setw(30) << reg.first << setw(14) << " = " << setw(8) << reg.second.reg_initial_value[0] << endl;
            } else {
                if (reg.second.reg_initial_value.size() > 1) {
                    cout << "  " << setw(30) << reg.first << "[" << setw(4) <<reg.second.reg_bit_width-1 << ":" << "0] = [";
                    for (int i=0; i<reg.second.reg_initial_value.size(); i++) {
                        cout << reg.second.reg_initial_value[i] << " ";
                    }
                    cout << "] " << "(range: " << setw(8) << reg.second.reg_value_min << " ~ " << setw(8) << reg.second.reg_value_max << ")" << endl;
                } else {
                    cout << "  " << setw(30) << reg.first << "[" << setw(4) << reg.second.reg_bit_width-1 << ":" << setw(4) << "0" << "] = " << setw(8) << reg.second.reg_initial_value[0] << " " << "(range: " << setw(8) << reg.second.reg_value_min << " ~ " << setw(8) << reg.second.reg_value_max << ")" << endl;
                }
            }
        }
    }

    // void randomize_check() const {
    //     for (auto reg : reg_map) {
    //         if (reg.second.reg_bit_width == 1) {
    //             if (reg.second.reg_initial_value[0] != 0 && reg.second.reg_initial_value[0] != 1) {
    //                 cout << "Error: " << reg.first << " initial value must be 0 or 1" << endl;
    //             }
    //         } else {
    //             if (reg.second.reg_initial_value.size() != reg.second.reg_bit_width) {
    //                 cout << "Error: " << reg.first << " initial value size must be " << reg.second.reg_bit_width << endl;
    //             }
    //         }
    //     }
    // }

};


inline void from_json(const json& j, RegisterSection& info) {
    for (auto& reg : j.items()) {
        from_json(reg.value(), info.reg_map[reg.key()]);
    }
    // from_json(j["reg_image_width"], info.reg_image_width);
    // from_json(j["reg_image_height"], info.reg_image_height);
    // from_json(j["reg_smooth_filter_enable"], info.reg_smooth_filter_enable);
    // from_json(j["reg_smooth_filter_coeff"], info.reg_smooth_filter_coeff);
    // from_json(j["reg_dpc_enable"], info.reg_dpc_enable);
    // from_json(j["reg_dpc_threshold"], info.reg_dpc_threshold);
    // from_json(j["reg_crop_enable"], info.reg_crop_enable);
    // from_json(j["reg_crop_start_x"], info.reg_crop_start_x);
    // from_json(j["reg_crop_start_y"], info.reg_crop_start_y);
    // from_json(j["reg_crop_end_x"], info.reg_crop_end_x);
    // from_json(j["reg_crop_end_y"], info.reg_crop_end_y);
}
        

        #define RANDOM_REG_ENABLE
inline void from_json(const json& j, RegisterInfo& reg) {
    reg.reg_bit_width = j["reg_bit_width"];
    reg.reg_value_min = j["reg_value_min"];
    reg.reg_value_max = j["reg_value_max"];
    
    #ifdef RANDOM_REG_ENABLE
    // randomize register section
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(reg.reg_value_min, reg.reg_value_max);
    reg.reg_initial_value = j["reg_initial_value"].get<std::vector<int>>();
    for (int i=0; i<reg.reg_initial_value.size(); i++) {
            reg.reg_initial_value[i] = dis(gen);
        }
        #else
        reg.reg_initial_value = j["reg_initial_value"].get<std::vector<int>>();
        #endif
    }
*/
    
// image_info loading
inline void from_json(const json& j, ImageSection& info) {
    info.src_image_format = j["src_image_format"];
    info.src_image_data_bitwidth = j["src_image_data_bitwidth"];
    info.generate_random_src_image_enable = j["generate_random_src_image_enable"];
}

// output_info loading
inline void from_json(const json& j, OutputSection& info) {
    info.src_image_path = j["src_image_path"];
    info.random_src_image_path = j["random_src_image_path"];
    info.alg_crop_output_path = j["alg_crop_output_path"];
    info.alg_dpc_output_path = j["alg_dpc_output_path"];
    info.py_dpc_output_path = j["py_dpc_output_path"];
    info.hls_crop_output_path = j["hls_crop_output_path"];
    info.hls_dpc_output_path = j["hls_dpc_output_path"];
}

inline ImageSection LoadImageConfigJsonImageSection(const string& filename) {
    ifstream f(filename);
    if (!f.is_open()) {
        MAIN_ERROR_1("Cannot open image_config.json configuration file");
    }
    json data = json::parse(f);
    f.close();
    ImageSection image_section = data["image_info"].get<ImageSection>();
    return image_section;
}

inline OutputSection LoadImageConfigJsonOutputSection(const string& filename) {
    ifstream f(filename);
    if (!f.is_open()) {
        MAIN_ERROR_1("Cannot open image_config.json configuration file");
    }
    json data = json::parse(f);
    f.close();
    OutputSection output_section = data["output_info"].get<OutputSection>();
    return output_section;
}


#endif