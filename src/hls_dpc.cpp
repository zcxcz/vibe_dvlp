#include "hls_dpc.h"
#include <hls_math.h>

// 辅助函数：获取带镜像边界的像素值
ap_uint<DATA_WIDTH> get_mirrored_pixel(ap_uint<DATA_WIDTH> image_buffer[4096][4096], int width, int height, int x, int y) {
    // 镜像边界处理
    if (x < 0) x = -x;
    if (x >= width) x = 2 * width - 1 - x;
    if (y < 0) y = -y;
    if (y >= height) y = 2 * height - 1 - y;
    return image_buffer[y][x];
}

// 与alg_dpc.cpp匹配的DPC滤波实现
void dpc_hls(
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
    const int MAX_WIDTH = 4096;  // 最大支持宽度
    const int MAX_HEIGHT = 4096; // 最大支持高度
    
    // 存储图像数据的缓冲区
    ap_uint<DATA_WIDTH> image_buffer[MAX_HEIGHT][MAX_WIDTH];
    #pragma HLS RESOURCE variable=image_buffer core=RAM_2P
    
    // 1. 读取输入流到缓冲区
    ap_uint<16> y, x;
    for(y = 0; y < regs.image_height; y++) {
        for(x = 0; x < regs.image_width; x++) {
            #pragma HLS PIPELINE II=1
            axis_pixel_t data_pkt = input_stream.read();
            image_buffer[y][x] = data_pkt.data;
        }
    }
    
    // 2. 应用DPC算法
    for(y = 0; y < regs.image_height; y++) {
        for(x = 0; x < regs.image_width; x++) {
            #pragma HLS PIPELINE II=1
            ap_uint<DATA_WIDTH> current_pixel = image_buffer[y][x];
            ap_uint<DATA_WIDTH> corrected_pixel = current_pixel;

            // --- 坏点检测 ---
            // 条件1: 中心像素值是否在其邻域的最大/最小值范围之外
            // 使用5x5窗口但只考虑特定位置的像素
            ap_uint<DATA_WIDTH> min_neighbor = 65535; // 假设DATA_WIDTH是16位
            ap_uint<DATA_WIDTH> max_neighbor = 0;

            // 5x5窗口中的特定位置
            int window_positions[8][2] = {
                {-2, -2}, {-2, 0}, {-2, 2},
                {0, -2},          {0, 2},
                {2, -2},  {2, 0}, {2, 2}
            };

            for (int i = 0; i < 8; ++i) {
                int nx = x + window_positions[i][0];
                int ny = y + window_positions[i][1];
                ap_uint<DATA_WIDTH> neighbor = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, nx, ny);
                if (neighbor < min_neighbor) min_neighbor = neighbor;
                if (neighbor > max_neighbor) max_neighbor = neighbor;
            }

            bool cond1_met = (current_pixel < min_neighbor) || (current_pixel > max_neighbor);

            // 条件2: 中心像素与所有8个邻居的差的绝对值是否都大于阈值
            bool cond2_met = true;
            if (cond1_met) {
                // 3x3邻域
                int neighbor_positions[8][2] = {
                    {-1, -1}, {-1, 0}, {-1, 1},
                    {0, -1},          {0, 1},
                    {1, -1},  {1, 0}, {1, 1}
                };

                for (int i = 0; i < 8; ++i) {
                    int nx = x + neighbor_positions[i][0];
                    int ny = y + neighbor_positions[i][1];
                    ap_uint<DATA_WIDTH> neighbor = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, nx, ny);
                    ap_int<DATA_WIDTH + 1> diff = (ap_int<DATA_WIDTH + 1>)current_pixel - (ap_int<DATA_WIDTH + 1>)neighbor;
                    if (hls::abs(diff) <= regs.dpc_threshold) {
                        cond2_met = false;
                        break;
                    }
                }
            } else {
                cond2_met = false;
            }

            // 如果两个条件都满足，则判定为坏点
            if (cond1_met && cond2_met) {
                // --- 坏点校正 ---
                // 计算四个方向的梯度
                // 垂直梯度
                ap_uint<DATA_WIDTH> p_up = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x, y-2);
                ap_uint<DATA_WIDTH> p_down = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x, y+2);
                ap_int<DATA_WIDTH + 2> dv = hls::abs(-(ap_int<DATA_WIDTH + 2>)p_up + 2*(ap_int<DATA_WIDTH + 2>)current_pixel - (ap_int<DATA_WIDTH + 2>)p_down);

                // 水平梯度
                ap_uint<DATA_WIDTH> p_left = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x-2, y);
                ap_uint<DATA_WIDTH> p_right = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x+2, y);
                ap_int<DATA_WIDTH + 2> dh = hls::abs(-(ap_int<DATA_WIDTH + 2>)p_left + 2*(ap_int<DATA_WIDTH + 2>)current_pixel - (ap_int<DATA_WIDTH + 2>)p_right);

                // 左对角线梯度 (左上-右下)
                ap_uint<DATA_WIDTH> p_ul = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x-2, y-2);
                ap_uint<DATA_WIDTH> p_dr = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x+2, y+2);
                ap_int<DATA_WIDTH + 2> ddl = hls::abs(-(ap_int<DATA_WIDTH + 2>)p_ul + 2*(ap_int<DATA_WIDTH + 2>)current_pixel - (ap_int<DATA_WIDTH + 2>)p_dr);

                // 右对角线梯度 (右上-左下)
                ap_uint<DATA_WIDTH> p_ur = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x+2, y-2);
                ap_uint<DATA_WIDTH> p_dl = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x-2, y+2);
                ap_int<DATA_WIDTH + 2> ddr = hls::abs(-(ap_int<DATA_WIDTH + 2>)p_ur + 2*(ap_int<DATA_WIDTH + 2>)current_pixel - (ap_int<DATA_WIDTH + 2>)p_dl);

                // 找到最小梯度
                ap_int<DATA_WIDTH + 2> min_grad = dv;
                if (dh < min_grad) min_grad = dh;
                if (ddl < min_grad) min_grad = ddl;
                if (ddr < min_grad) min_grad = ddr;

                // 沿最小梯度方向进行插值
                ap_uint<DATA_WIDTH> new_p0;
                if (min_grad == dv) {
                    // 垂直方向
                    ap_uint<DATA_WIDTH> p_up1 = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x, y-1);
                    ap_uint<DATA_WIDTH> p_down1 = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x, y+1);
                    new_p0 = (p_up1 + p_down1) / 2;
                } else if (min_grad == dh) {
                    // 水平方向
                    ap_uint<DATA_WIDTH> p_left1 = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x-1, y);
                    ap_uint<DATA_WIDTH> p_right1 = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x+1, y);
                    new_p0 = (p_left1 + p_right1) / 2;
                } else if (min_grad == ddl) {
                    // 左对角线方向
                    ap_uint<DATA_WIDTH> p_ul1 = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x-1, y-1);
                    ap_uint<DATA_WIDTH> p_dr1 = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x+1, y+1);
                    new_p0 = (p_ul1 + p_dr1) / 2;
                } else { // min_grad == ddr
                    // 右对角线方向
                    ap_uint<DATA_WIDTH> p_ur1 = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x+1, y-1);
                    ap_uint<DATA_WIDTH> p_dl1 = get_mirrored_pixel(image_buffer, regs.image_width, regs.image_height, x-1, y+1);
                    new_p0 = (p_ur1 + p_dl1) / 2;
                }

                corrected_pixel = new_p0;
            }
            
            // 4. 写入输出流
            axis_pixel_t output_pkt;
            output_pkt.data = corrected_pixel;
            output_pkt.keep = -1;  // 全字节有效
            output_pkt.strb = -1;
            output_pkt.last = (y == regs.image_height - 1) && (x == regs.image_width - 1);
            
            output_stream.write(output_pkt);
        }
    }
}