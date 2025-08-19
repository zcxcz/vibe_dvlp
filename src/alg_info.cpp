#include "alg_info.h"

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

// RegInfo implementation
int RegInfo::operator[](size_t index) const {
    return index < reg_initial_value.size() ? reg_initial_value[index] : 0;
}

size_t RegInfo::size() const {
    return reg_initial_value.empty() ? 1 : reg_initial_value.size();
}

void RegInfo::print_values(const string& name) const {
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

// AlgCommonRegisterInfo implementation
void AlgCommonRegisterInfo::print_info() const {
    cout << "=== Common Register Information ===" << endl;
    print_reg("Reg Image Width", reg_image_width);
    print_reg("Reg Image Height", reg_image_height);
    cout << "==================================" << endl;
}

void AlgCommonRegisterInfo::print_reg(const string& name, const RegInfo& reg) const {
    print_reg_value(name, reg);
    cout << "  Bit Width: " << reg.reg_bit_width << endl;
    cout << "  Min Value: " << reg.reg_value_min << endl;
    cout << "  Max Value: " << reg.reg_value_max << endl;
    reg.print_values("  Initial Value");
}

void AlgCommonRegisterInfo::print_reg_value(const string& name, const RegInfo& reg) const {
    cout << name << ": ";
    if(reg.reg_initial_value.empty()) {
        cout << "0";
    } else if (reg.reg_initial_value.size() == 1) {
        cout << reg.reg_initial_value[0];
    } else {
        for (size_t i = 0; i < reg.reg_initial_value.size(); ++i) {
            cout << reg.reg_initial_value[i];
            if (i < reg.reg_initial_value.size() - 1) cout << " ";
        }
    }
    cout << endl;
}

// AlgCropRegisterInfo implementation
void AlgCropRegisterInfo::print_values() const {
    cout << "=== Crop Register Information ===" << endl;
    print_reg_info("Reg Crop Enable", reg_crop_enable);
    print_reg_info("Reg Crop Start X", reg_crop_start_x);
    print_reg_info("Reg Crop Start Y", reg_crop_start_y);
    print_reg_info("Reg Crop End X", reg_crop_end_x);
    print_reg_info("Reg Crop End Y", reg_crop_end_y);
    cout << "=================================" << endl;
}

void AlgCropRegisterInfo::print_reg_info(const string& name, const RegInfo& reg) const {
    cout << name << ":" << endl;
    cout << "  Bit Width: " << reg.reg_bit_width << endl;
    cout << "  Min Value: " << reg.reg_value_min << endl;
    cout << "  Max Value: " << reg.reg_value_max << endl;
    reg.print_values("  Initial Value");
}

// AlgDpcRegisterInfo implementation
void AlgDpcRegisterInfo::print_values() const {
    cout << "=== DPC Register Information ===" << endl;
    print_reg_info("Reg DPC Enable", reg_dpc_enable);
    print_reg_info("Reg DPC Threshold", reg_dpc_threshold);
    cout << "===============================" << endl;
}

void AlgDpcRegisterInfo::print_reg_info(const string& name, const RegInfo& reg) const {
    cout << name << ":" << endl;
    cout << "  Bit Width: " << reg.reg_bit_width << endl;
    cout << "  Min Value: " << reg.reg_value_min << endl;
    cout << "  Max Value: " << reg.reg_value_max << endl;
    reg.print_values("  Initial Value");
}
