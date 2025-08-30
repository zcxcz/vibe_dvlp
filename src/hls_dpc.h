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
    // 行缓冲区定义 - 使用与hls_crop.h中一致的类型
    static ap_uint<DATA_WIDTH*2> hls_dpc_linebuffer[4][(HLS_DPC_LINEBUFFER_DEPTH+1)/2];
    
    // 辅助函数：裁剪值到指定范围
    ap_uint<16> clip(ap_uint<16> value, ap_uint<16> min, ap_uint<16> max);
    
    // 内部DPC处理函数
    void ProcessDpc(ap_uint<DATA_WIDTH>* pixel_window, ap_uint<DATA_WIDTH>& pixel_out, const HlsDpcRegisterInfo& regs);
    
public:
    HlsDpc() {};
    ~HlsDpc() {};
    
    // DPC处理函数
    void Process(
        hls::stream<axis_pixel_t>& input_stream,
        hls::stream<axis_pixel_t>& output_stream,
        const HlsDpcRegisterInfo& regs
    );
};

#endif