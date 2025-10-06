#ifndef ALG_INFO_H
#define ALG_INFO_H

// std
#include <string>

using std::string;

// Forward declarations
struct AlgRegisterSection {
    // register info
    int reg_image_width;
    int reg_image_height;
    int reg_crop_start_x;
    int reg_crop_start_y;
    int reg_crop_end_x;
    int reg_crop_end_y;
    bool reg_crop_enable;
    bool reg_dpc_enable;
    int reg_dpc_threshold;
};

struct AlgImageSection {
    // image info
    string image_path;
    string random_image_path;
    int generate_random_image;
};

struct AlgOutputSection {
    // image info
    string alg_crop_output_path;
    string alg_dpc_output_path;
    string hls_crop_output_path;
    string hls_dpc_output_path;
};
    
#endif // ALG_INFO_H
