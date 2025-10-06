#ifndef HLS_INFO_H
#define HLS_INFO_H

// std
#include <string>

// ip
#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"

// using
using std::string;


struct HlsRegisterSection {
    ap_uint<16> reg_image_width;
    ap_uint<16> reg_image_height;
    ap_uint<1>  reg_crop_enable;
    ap_uint<16> reg_crop_start_x;
    ap_uint<16> reg_crop_start_y;
    ap_uint<16> reg_crop_end_x;
    ap_uint<16> reg_crop_end_y;
    ap_uint<1>  reg_dpc_enable;
    ap_uint<16> reg_dpc_threshold;
};

struct HlsImageSection {
    // image info
    string image_path;
    string random_image_path;
    int generate_random_image;
};

struct HlsOutputSection {
    // image info
    string alg_crop_output_path;
    string alg_dpc_output_path;
    string hls_crop_output_path;
    string hls_dpc_output_path;
};


#endif