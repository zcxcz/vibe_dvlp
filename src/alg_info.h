#ifndef ALG_INFO_H
#define ALG_INFO_H

#include <vector>
#include <string>
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


struct RegisterParameter {
    int reg_bit_width;
    int reg_value_min;
    int reg_value_max;
    vector<int> reg_initial_value;

    int operator[](size_t index) const;
    size_t size() const;
    void set_value(size_t index, int value);
    void set_values(const std::vector<int>& values);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RegisterParameter, reg_bit_width, reg_value_min, reg_value_max, reg_initial_value)

};


struct RegisterInfo {
    RegisterParameter reg_image_width;
    RegisterParameter reg_image_height;
    // crop
    RegisterParameter reg_crop_enable;
    RegisterParameter reg_crop_start_x;
    RegisterParameter reg_crop_start_y;
    RegisterParameter reg_crop_end_x;
    RegisterParameter reg_crop_end_y;
    // dpc
    RegisterParameter reg_dpc_enable;
    RegisterParameter reg_dpc_threshold;
    
    void print_values() const;
    void print_reg(const string& reg_name, const RegisterParameter& reg_param) const;
    void set_value(size_t index, int value);
    void set_values(const std::vector<int>& values);
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RegisterInfo, reg_image_width, reg_image_height, reg_crop_enable, reg_crop_start_x, reg_crop_start_y, reg_crop_end_x, reg_crop_end_y, reg_dpc_enable, reg_dpc_threshold)
    
};
    
#endif // ALG_INFO_H
