#ifndef HLS_CROP_H
#define HLS_CROP_H

// std
#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"

// using
using std::string;

// ip
#include "hls_info.h"


template <int HLS_INPUT_DATA_BITWIDTH, int HLS_OUTPUT_DATA_BITWIDTH>
class HlsCrop {
public:
    HlsCrop() {};
    ~HlsCrop() {};  

    void run(
        hls::stream<ap_axiu<HLS_INPUT_DATA_BITWIDTH, 0, 0, 0>>& input_stream,
        hls::stream<ap_axiu<HLS_OUTPUT_DATA_BITWIDTH, 0, 0, 0>>& output_stream,
        const HlsRegisterSection& hls_register_section
    ) {
        #pragma HLS INTERFACE axis port=input_stream
        #pragma HLS INTERFACE axis port=output_stream
        #pragma HLS INTERFACE s_axilite port=hls_register_section bundle=control
        #pragma HLS INTERFACE s_axilite port=return bundle=control
        
        // 防止编译器优化掉未使用的regs
        HlsRegisterSection dummy = hls_register_section;

        if (!hls_register_section.reg_crop_enable) {
            // 如果crop未启用，直接透传所有数据（包括last信号）
            // 使用两层for循环实现数据透传，与裁剪模式结构一致
            ap_uint<16> y_cnt = 0;
            ap_uint<16> x_cnt = 0;
            
            for(y_cnt = 0; y_cnt < hls_register_section.reg_image_height; y_cnt++) {
                for(x_cnt = 0; x_cnt < hls_register_section.reg_image_width; x_cnt++) {
                    #pragma HLS PIPELINE II=1
                    ap_axiu<HLS_INPUT_DATA_BITWIDTH, 0, 0, 0> data_pkt = input_stream.read();
                    output_stream.write(data_pkt);
                }
            }
            return;
        }
        
        // 处理每个像素
        ap_uint<16> y_cnt = 0;
        ap_uint<16> x_cnt = 0;
        ap_uint<32> output_count = 0;
        ap_uint<32> expected_output = (ap_uint<32>(hls_register_section.reg_crop_end_y) - hls_register_section.reg_crop_start_y + 1) * 
                                    (ap_uint<32>(hls_register_section.reg_crop_end_x) - hls_register_section.reg_crop_start_x + 1);
        
        // 需要跟踪当前帧是否结束
        bool frame_end = false;
        
        for(y_cnt=0; y_cnt<hls_register_section.reg_image_height; y_cnt++){
            for(x_cnt=0; x_cnt<hls_register_section.reg_image_width; x_cnt++){
                #pragma HLS PIPELINE II=1
                ap_axiu<HLS_INPUT_DATA_BITWIDTH, 0, 0, 0> data_pkt = input_stream.read();
                
                // 检查是否在裁剪区域内
                bool x_in_range = (x_cnt >= hls_register_section.reg_crop_start_x && x_cnt <= hls_register_section.reg_crop_end_x);
                bool y_in_range = (y_cnt >= hls_register_section.reg_crop_start_y && y_cnt <= hls_register_section.reg_crop_end_y);
                bool in_crop_region = (x_in_range && y_in_range);
                
                // 检查是否到达帧尾
                if (data_pkt.last == 1) {
                    frame_end = true;
                }
                
                if (in_crop_region) {
                    output_count++;
                }
                
                bool crop_count_end = (output_count == expected_output);
                
                // 如果是裁剪区域的最后一个像素，设置last信号
                if (crop_count_end) {
                    data_pkt.last = 1;
                } else if (!frame_end) {
                    // 如果不是原始帧的结束，保持last为0
                    data_pkt.last = 0;
                }
                // 如果是原始帧的结束且在裁剪区域外，则不修改last信号

                if (in_crop_region) {
                    output_stream.write(data_pkt);
                }
            }
        }
    }


};

#endif