#include "crop.h"
#include <hls_stream.h>

void crop_hls(
    hls::stream<pixel_t>& input_stream,
    hls::stream<pixel_t>& output_stream,
    const RegisterHlsInfo& regs
) {
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=regs bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
    if (!regs.crop_enable) {
        // 如果crop未启用，直接透传数据
        while (!input_stream.empty()) {
            #pragma HLS PIPELINE II=1
        pixel_t data = input_stream.read();
        output_stream.write(data);
        }
        return;
    }
    
    // 处理每个像素
    ap_uint<16> y_cnt = 0;
    ap_uint<16> x_cnt = 0;
    
    for (ap_uint<32> i = 0; i < regs.image_width * regs.image_height; i++) {
        #pragma HLS PIPELINE II=1
        
        if (!input_stream.empty()) {
            pixel_t data_in = input_stream.read();
            
            // 检查是否在裁剪区域内
            bool x_in_range = (x_cnt >= regs.crop_start_x && x_cnt <= regs.crop_end_x);
            bool y_in_range = (y_cnt >= regs.crop_start_y && y_cnt <= regs.crop_end_y);
            bool in_crop_region = (x_in_range && y_in_range);
            
            if (in_crop_region) {
                output_stream.write(data_in);
            }
            
            // 更新行列计数 - 正确的行列遍历
            x_cnt++;
            if (x_cnt == regs.image_width) {
                x_cnt = 0;
                y_cnt++;
            }
        }
    }
}