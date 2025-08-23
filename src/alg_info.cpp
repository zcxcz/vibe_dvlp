#include "alg_info.h"
#include <iostream>

// ImageInfo implementation
void ImageInfo::print_values() const {
    cout << "=== Image Information ===" << endl;
    cout << "Image Path: " << image_path << endl;
    cout << "Image Format: " << image_format << endl;
    cout << "Image Data Bitwidth: " << image_data_bitwidth << endl;
    cout << "Generate Random Image: " << (generate_random_image ? "Yes" : "No") << endl;
    cout << "Random Image Path: " << random_image_path << endl;
    cout << "========================" << endl;
}


// RegisterParameter implementation
void RegisterInfo::print_values() const {
    cout << "=== Common Register Information ===" << endl;
    print_reg("Reg Image Width", reg_image_width);
    print_reg("Reg Image Height", reg_image_height);
    cout << "=== Crop Register Information ===" << endl;
    print_reg("Reg Crop Enable", reg_crop_enable);
    print_reg("Reg Crop Start X", reg_crop_start_x);
    print_reg("Reg Crop Start Y", reg_crop_start_y);
    print_reg("Reg Crop End X", reg_crop_end_x);
    print_reg("Reg Crop End Y", reg_crop_end_y);
    cout << "=== DPC Register Information ===" << endl;
    print_reg("Reg DPC Enable", reg_dpc_enable);
    print_reg("Reg DPC Threshold", reg_dpc_threshold);
    cout << "===============================" << endl;
}
void RegisterInfo::print_reg(const string& reg_name, const RegisterParameter& reg_param) const {
    cout << "reg_bit_width: " << reg_param.reg_bit_width << endl;
    cout << "reg_value_min: " << reg_param.reg_value_min << endl;
    cout << "reg_value_max: " << reg_param.reg_value_max << endl;
    if (reg_param.reg_initial_value.size() == 1) {
        cout << reg_name << "initial value: " << reg_param.reg_initial_value[0] << endl;
    } else {
        cout << reg_name << "initial value: " ;
        for (size_t i = 0; i < reg_param.reg_initial_value.size(); i++) {
            cout << reg_param.reg_initial_value[i] << " ";
        }
        cout << endl;
    }
}


int RegisterParameter::operator[](size_t index) const {
    return index < reg_initial_value.size() ? reg_initial_value[index] : 0;
}
size_t RegisterParameter::size() const {
    return reg_initial_value.empty() ? 1 : reg_initial_value.size();
}
void RegisterParameter::set_value(size_t index, int value) {
    if (index < reg_initial_value.size()) {
        reg_initial_value[index] = value;
    }
}
void RegisterParameter::set_values(const std::vector<int>& values) {
    reg_initial_value = values;
}