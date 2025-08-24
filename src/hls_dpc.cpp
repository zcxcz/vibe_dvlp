#include "hls_dpc.h"
#include <hls_math.h>

// 定义静态成员变量
ap_uint<DATA_WIDTH> HlsDpc::hls_dpc_linebuffer[5][HLS_DPC_LINEBUFFER_DEPTH] = {};

// HlsDpc类的私有方法实现：获取带镜像边界的像素值
ap_uint<DATA_WIDTH> HlsDpc::get_mirrored_pixel(int width, int height, int x, int y) {
    // 镜像边界处理
    if (x < 0) x = -x;
    if (x >= width) x = 2 * width - 1 - x;
    if (y < 0) y = -y;
    if (y >= height) y = 2 * height - 1 - y;
    return hls_dpc_linebuffer[y][x];
}

// HlsDpc类的私有方法实现：裁剪值到指定范围
ap_uint<16> HlsDpc::clip(ap_uint<16> value, ap_uint<16> min, ap_uint<16> max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// HlsDpc类的process方法实现
void HlsDpc::process(
    hls::stream<axis_pixel_t>& input_stream,
    hls::stream<axis_pixel_t>& output_stream,
    const HlsDpcRegisterInfo& regs
) {
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=regs bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    // 如果DPC未启用，直接透传数据
    if (!regs.dpc_enable) {
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

    // DPC处理
    
    // 存储图像数据的缓冲区
    #pragma HLS RESOURCE variable=hls_dpc_linebuffer core=RAM_2P
    
    // 1. 读取输入流到缓冲区
    ap_uint<16> cnt_x=0;
    ap_uint<16> cnt_y=0;
    ap_uint<DATA_WIDTH> pixel_data[9];
    for (cnt_y = 0; cnt_y < regs.image_height+2; cnt_y++) {
        for (cnt_x = 0; cnt_x < regs.image_width+2; cnt_x++) {
            #pragma HLS PIPELINE II=1
            if (cnt_y < regs.image_height && cnt_x < regs.image_width) {
                axis_pixel_t data_pkt = input_stream.read();
                hls_dpc_linebuffer[cnt_y%5][cnt_x] = data_pkt.data;
            }
            if (cnt_y>1 && cnt_x>1) {
                // upd pixel data
                ap_uint<4> pixel_index = 0;
                for (int j=cnt_y-4; j<=cnt_y; j=j+2) {
                    for (int i=cnt_x-4; i<=cnt_x; i=i+2) {
                        #pragma HLS PIPELINE II=1
                        pixel_data[pixel_index] = hls_dpc_linebuffer[clip(j)%5][clip(i)];
                        pixel_index++;
                    }
                }

                // 条件1: 中心像素是否小于或大于其8个邻居中的最小值或最大值
                ap_uint<DATA_WIDTH> min_neighbor = pixel_data[0];
                ap_uint<DATA_WIDTH> max_neighbor = pixel_data[0];
                for (int i = 1; i < 9; ++i) {
                    #pragma HLS PIPELINE II=1
                    if (pixel_data[i] < min_neighbor) min_neighbor = pixel_data[i];
                    if (pixel_data[i] > max_neighbor) max_neighbor = pixel_data[i];
                }
                bool cond1_met = (pixel_data[4] < min_neighbor) || (pixel_data[4] > max_neighbor);

            // 条件2: 中心像素与所有8个邻居的差的绝对值是否都大于阈值
            bool cond2_met = true;
            if (cond1_met) {
                // 3x3邻域
                ap_uint<DATA_WIDTH> abs_diff_2p[9];
                ap_int<DATA_WIDTH+1> diff_2p[9];
                for (int i = 0; i < 9; ++i) {
                    diff_2p[i] = (ap_int<DATA_WIDTH+1>)pixel_data[i] - (ap_int<DATA_WIDTH+1>)pixel_data[4];
                    abs_diff_2p[i] = hls::abs(diff_2p[i]);
                    if ( (i!=4) && (abs_diff_2p[i] <= regs.dpc_threshold) ) {
                        cond2_met = false;
                        break;
                    }
                }
            } else {
                cond2_met = false;
            }

            // 初始化校正像素为中心像素
            ap_uint<DATA_WIDTH> corrected_pixel = pixel_data[4];

            // 如果两个条件都满足，则判定为坏点并校正

            // 如果两个条件都满足，则判定为坏点
            if (cond1_met && cond2_met) {
                // --- 坏点校正 ---
                // 计算四个方向的梯度
                ap_int<DATA_WIDTH + 2> dv = hls::abs(-(ap_int<DATA_WIDTH + 2>)pixel_data[1] + 2*(ap_int<DATA_WIDTH + 2>)pixel_data[4] - (ap_int<DATA_WIDTH + 2>)pixel_data[7]);
                ap_int<DATA_WIDTH + 2> dh = hls::abs(-(ap_int<DATA_WIDTH + 2>)pixel_data[3] + 2*(ap_int<DATA_WIDTH + 2>)pixel_data[4] - (ap_int<DATA_WIDTH + 2>)pixel_data[5]);
                ap_int<DATA_WIDTH + 2> ddr = hls::abs(-(ap_int<DATA_WIDTH + 2>)pixel_data[0] + 2*(ap_int<DATA_WIDTH + 2>)pixel_data[4] - (ap_int<DATA_WIDTH + 2>)pixel_data[8]);
                ap_int<DATA_WIDTH + 2> ddl = hls::abs(-(ap_int<DATA_WIDTH + 2>)pixel_data[2] + 2*(ap_int<DATA_WIDTH + 2>)pixel_data[4] - (ap_int<DATA_WIDTH + 2>)pixel_data[6]);

                // 找到最小梯度
                ap_int<DATA_WIDTH + 2> min_grad = dv;
                if (dh < min_grad) min_grad = dh;
                if (ddl < min_grad) min_grad = ddl;
                if (ddr < min_grad) min_grad = ddr;

                // 计算中心像素的坐标
                ap_uint<16> x = cnt_x - 2;
                ap_uint<16> y = cnt_y - 2;
                
                // 应用最小梯度方向的修正
                if (min_grad == dv) {
                    // 垂直方向修正 (上+下)
                    ap_uint<DATA_WIDTH> new_p0 = (pixel_data[1] + pixel_data[7]) / 2;
                    corrected_pixel = new_p0;
                } else if (min_grad == dh) {
                    // 水平方向修正
                    ap_uint<DATA_WIDTH> new_p0 = (pixel_data[3] + pixel_data[5]) / 2;
                    corrected_pixel = new_p0;
                } else if (min_grad == ddl) {
                    // 左对角线方向修正
                    ap_uint<DATA_WIDTH> new_p0 = (pixel_data[0] + pixel_data[8]) / 2;
                    corrected_pixel = new_p0;
                } else if (min_grad == ddr) {
                    // 右对角线方向修正
                    ap_uint<DATA_WIDTH> new_p0 = (pixel_data[2] + pixel_data[6]) / 2;
                    corrected_pixel = new_p0;
                }
            } else {
                corrected_pixel = pixel_data[4];
            }
                
                // 计算当前输出像素的坐标
                ap_uint<16> x_out = cnt_x - 2;
                ap_uint<16> y_out = cnt_y - 2;

                // 确保坐标在有效范围内
                x_out = clip(x_out, 0, regs.image_width - 1);
                y_out = clip(y_out, 0, regs.image_height - 1);

                // 5. 写入输出流
                axis_pixel_t output_pkt;
                output_pkt.data = corrected_pixel;
                output_pkt.keep = -1;  // 全字节有效
                output_pkt.strb = -1;
                output_pkt.last = (y_out == regs.image_height - 1) && (x_out == regs.image_width - 1);
                
                output_stream.write(output_pkt);
        }
    }
}