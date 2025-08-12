#ifndef CROP_H
#define CROP_H

#include <ap_int.h>
#include <hls_stream.h>

// 定义数据类型为10-bit
const int DATA_WIDTH = 10;
typedef ap_uint<DATA_WIDTH> pixel_t;

#include "ap_axi_sdata.h"

// 使用标准的AXI Stream数据类型，兼容模板工程
typedef ap_axiu<10, 0, 0, 0> axis_pixel_t;

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
    hls::stream<axis_pixel_t>& input_stream,
    hls::stream<axis_pixel_t>& output_stream,
    const RegisterHlsInfo& regs
);

#endif