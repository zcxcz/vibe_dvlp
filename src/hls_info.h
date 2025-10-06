#ifndef HLS_INFO_H
#define HLS_INFO_H

#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"


struct HlsRegisterSection {
    ap_uint<16> image_width;
    ap_uint<16> image_height;
    ap_uint<1>  crop_enable;
    ap_uint<16> crop_start_x;
    ap_uint<16> crop_start_y;
    ap_uint<16> crop_end_x;
    ap_uint<16> crop_end_y;
    ap_uint<1>  dpc_enable;
    ap_uint<16> dpc_threshold;
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