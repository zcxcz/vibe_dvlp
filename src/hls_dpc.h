#ifndef HLS_DPC_H
#define HLS_DPC_H

#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include "hls_crop.h"

#define HLS_DPC_LINEBUFFER_DEPTH 1024

// HLS DPC寄存器信息结构体
struct HlsDpcRegisterInfo {
    ap_uint<16> image_width;
    ap_uint<16> image_height;
    ap_uint<1>  dpc_enable;
    ap_uint<16> dpc_threshold;
};

// HLS DPC类
class HlsDpc {
private:
    // 辅助函数：获取带镜像边界的像素值
    static ap_uint<DATA_WIDTH> hls_dpc_linebuffer[4][HLS_DPC_LINEBUFFER_DEPTH];
    
    // 辅助函数：裁剪值到指定范围
    ap_uint<16> clip(ap_uint<16> value, ap_uint<16> min, ap_uint<16> max);
    
    public:
    HlsDpc() {};
    ~HlsDpc() {};
    
    // DPC处理函数
    void process(
        hls::stream<axis_pixel_t>& input_stream,
        hls::stream<axis_pixel_t>& output_stream,
        const HlsDpcRegisterInfo& regs
    );
};

#endif