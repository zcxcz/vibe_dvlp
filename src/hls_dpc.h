#ifndef HLS_DPC_H
#define HLS_DPC_H

#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "hls_crop.h"

// HLS DPC寄存器信息结构体
struct HlsDpcRegisterInfo {
    ap_uint<16> image_width;
    ap_uint<16> image_height;
    ap_uint<1>  dpc_enable;
    ap_uint<16> dpc_threshold;
};

// HLS DPC顶层函数声明
void dpc_hls(
    hls::stream<axis_pixel_t>& input_stream,
    hls::stream<axis_pixel_t>& output_stream,
    const HlsDpcRegisterInfo& regs
);

#endif