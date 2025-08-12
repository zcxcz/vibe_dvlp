#include "hls_crop.h"
#include <hls_stream.h>

void crop_hls(
    hls::stream<axis_pixel_t>& input_stream,
    hls::stream<axis_pixel_t>& output_stream,
    const RegisterHlsInfo& regs
) {
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=regs bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
    ap_uint<32> total_pixels = regs.image_height * regs.image_width;
    
    if (!regs.crop_enable) {
        // 如果crop未启用，直接透传所有数据（包括last信号）
        // 使用两层for循环实现数据透传，与裁剪模式结构一致
        ap_uint<16> y_cnt = 0;
        ap_uint<16> x_cnt = 0;
        
        for(y_cnt = 0; y_cnt < regs.image_height; y_cnt++) {
            for(x_cnt = 0; x_cnt < regs.image_width; x_cnt++) {
                #pragma HLS PIPELINE II=1
                axis_pixel_t data_pkt = input_stream.read();
                output_stream.write(data_pkt);
            }
        }
        return;
    }
    
    // 处理每个像素
    ap_uint<16> y_cnt = 0;
    ap_uint<16> x_cnt = 0;
    ap_uint<32> output_count = 0;
    ap_uint<32> expected_output = (regs.crop_end_y - regs.crop_start_y + 1) * 
                                  (regs.crop_end_x - regs.crop_start_x + 1);
    
    // 需要跟踪当前帧是否结束
    bool frame_end = false;
    
    for(y_cnt=0; y_cnt<regs.image_height; y_cnt++){
        for(x_cnt=0; x_cnt<regs.image_width; x_cnt++){
            #pragma HLS PIPELINE II=1
            axis_pixel_t data_pkt = input_stream.read();
            
            // 检查是否在裁剪区域内
            bool x_in_range = (x_cnt >= regs.crop_start_x && x_cnt <= regs.crop_end_x);
            bool y_in_range = (y_cnt >= regs.crop_start_y && y_cnt <= regs.crop_end_y);
            bool in_crop_region = (x_in_range && y_in_range);
            
            // 检查是否到达帧尾
            if (data_pkt.last == 1) {
                frame_end = true;
            }
            
            if (in_crop_region) {
                output_count++;
                
                // 在裁剪模式下，last信号处理策略：
                // 1. 如果检测到输入last信号，强制设置输出last
                // 2. 如果达到预期输出数量，设置last信号
                bool should_set_last = (output_count == expected_output) || frame_end;
                
                data_pkt.last = should_set_last ? 1 : 0;
                
                output_stream.write(data_pkt);
                
                // 如果已经到达帧尾且还有剩余数据，需要处理边界情况
                if (frame_end && output_count < expected_output) {
                    // 这种情况表示配置与输入数据不符，但已设置last信号
                    // DMA会根据last信号结束传输
                }
            }
            
            // 如果到达帧尾但还有输入数据，继续处理直到帧结束
            if (frame_end && !in_crop_region) {
                // 消费剩余数据但不输出
            }
        }
    }
}