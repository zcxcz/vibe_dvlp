#ifndef CROP_H
#define CROP_H

#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"

// 定义数据类型为10-bit
const int DATA_WIDTH = 8;
typedef ap_axiu<DATA_WIDTH, 0, 0, 0> axis_pixel_t;

// HLS寄存器信息结构体
struct HlsRegisterInfo {
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
// HLS寄存器信息结构体
struct HlsCropRegisterInfo {
    ap_uint<16> image_width;
    ap_uint<16> image_height;
    ap_uint<16> crop_start_x;
    ap_uint<16> crop_start_y;
    ap_uint<16> crop_end_x;
    ap_uint<16> crop_end_y;
    ap_uint<1>  crop_enable;
};

// HLS顶层函数声明
void crop_hls(
    hls::stream<axis_pixel_t>& input_stream,
    hls::stream<axis_pixel_t>& output_stream,
    const HlsCropRegisterInfo& regs
);

#endif