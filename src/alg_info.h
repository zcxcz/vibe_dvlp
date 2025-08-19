#ifndef ALG_INFO_H
#define ALG_INFO_H

#include <vector>
#include <string>
#include <iostream>
#include "json.hpp"

using namespace std;

struct ImageInfo {
    string image_path;
    string image_format;
    int image_data_bitwidth;
    int generate_random_image;
    string random_image_path;
    
    void print_values() const;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ImageInfo, image_path, image_format, image_data_bitwidth, generate_random_image, random_image_path)
};

struct RegInfo {
    int reg_bit_width;
    vector<int> reg_initial_value;
    int reg_value_min;
    int reg_value_max;
    
    int operator[](size_t index) const;
    size_t size() const;
    void print_values(const string& name = "Values") const;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RegInfo, reg_bit_width, reg_initial_value, reg_value_min, reg_value_max)
};

struct AlgCommonRegisterInfo {
    RegInfo reg_image_width;
    RegInfo reg_image_height;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AlgCommonRegisterInfo, reg_image_width, reg_image_height)
    
    void print_values() const;
    
private:
    void print_reg_info(const string& name, const RegInfo& reg) const;
};

struct AlgCropRegisterInfo {
    RegInfo reg_crop_enable;
    RegInfo reg_crop_start_x;
    RegInfo reg_crop_start_y;
    RegInfo reg_crop_end_x;
    RegInfo reg_crop_end_y;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AlgCropRegisterInfo, reg_crop_enable, reg_crop_start_x, reg_crop_start_y, reg_crop_end_x, reg_crop_end_y)
    
    void print_values() const;
    
private:
    void print_reg_info(const string& name, const RegInfo& reg) const;
};

struct AlgDpcRegisterInfo {
    RegInfo reg_dpc_enable;
    RegInfo reg_dpc_threshold;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AlgDpcRegisterInfo, reg_dpc_enable, reg_dpc_threshold)
    
    void print_values() const;
    
private:
    void print_reg_info(const string& name, const RegInfo& reg) const;
};


#endif // ALG_INFO_H
