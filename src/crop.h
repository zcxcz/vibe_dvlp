#ifndef CROP_H
#define CROP_H

#include <ap_int.h>
#include <hls_stream.h>

// 定义数据类型为10-bit
const int DATA_WIDTH = 10;
typedef ap_uint<DATA_WIDTH> pixel_t;

// HLS寄存器信息结构体
struct RegisterHlsInfo {
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
    hls::stream<pixel_t>& input_stream,
    hls::stream<pixel_t>& output_stream,
    const RegisterHlsInfo& regs
);

#endif