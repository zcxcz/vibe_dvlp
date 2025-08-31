#include "hls_dpc.h"
#include "hls_crop.h"
#include <hls_math.h>

using namespace std;

// 定义静态成员变量
ap_uint<DATA_WIDTH*2> HlsDpc::hls_dpc_linebuffer[4][(HLS_DPC_LINEBUFFER_DEPTH+1)/2] = {};

// HlsDpc类的私有方法实现：裁剪值到指定范围
ap_uint<16> HlsDpc::clip(ap_uint<16> value, ap_uint<16> min, ap_uint<16> max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void HlsDpc::ProcessDpc(ap_uint<DATA_WIDTH>* pixel_window, ap_uint<DATA_WIDTH>& pixel_out, const HlsDpcRegisterInfo& regs) {

    // 初始化校正像素为中心像素
    pixel_out = pixel_window[4];
    // min/max judgement
    ap_int<DATA_WIDTH> min_stg_1[4] = {};
    ap_int<DATA_WIDTH> max_stg_1[4] = {};
    ap_int<DATA_WIDTH> min_stg_2[2] = {};
    ap_int<DATA_WIDTH> max_stg_2[2] = {};
    ap_int<DATA_WIDTH> min_stg_3[1] = {};
    ap_int<DATA_WIDTH> max_stg_3[1] = {};
    for (int i=0; i<4; i++) {
        min_stg_1[i] = pixel_window[i] >= pixel_window[i+4] ? pixel_window[i+4] : pixel_window[i];
        max_stg_1[i] = pixel_window[i] >= pixel_window[i+4] ? pixel_window[i] : pixel_window[i+4];
    }
    for (int i=0; i<2; i++) {
        min_stg_2[i] = min_stg_1[i*2] >= min_stg_1[i*2+1] ? min_stg_1[i*2+1] : min_stg_1[i*2];
        max_stg_2[i] = max_stg_1[i*2] >= max_stg_1[i*2+1] ? max_stg_1[i*2] : max_stg_1[i*2+1];
    }
    min_stg_3[0] = min_stg_2[0] >= min_stg_2[1] ? min_stg_2[1] : min_stg_2[0];
    max_stg_3[0] = max_stg_2[0] >= max_stg_2[1] ? max_stg_2[0] : max_stg_2[1];
    ap_int<DATA_WIDTH> min_neighbor = min_stg_3[0];
    ap_int<DATA_WIDTH> max_neighbor = max_stg_3[0];
    bool cond1_met = (pixel_window[4] < min_neighbor) || (pixel_window[4] > max_neighbor);
    
    // threshold judgement
    bool cond2_met = false;
    ap_int<DATA_WIDTH+1> diff_2p[8];
    ap_uint<DATA_WIDTH+1> abs_diff_2p[8];
    ap_int<8> diff_2p_gt_thr;
    if (cond1_met) {
        for (int i = 0; i < 4; ++i) {
            diff_2p[i] = (ap_int<DATA_WIDTH+1>)pixel_window[i] - (ap_int<DATA_WIDTH+1>)pixel_window[4];
            diff_2p[i+4] = (ap_int<DATA_WIDTH+1>)pixel_window[i+4] - (ap_int<DATA_WIDTH+1>)pixel_window[4];
            abs_diff_2p[i] = hls::abs(diff_2p[i]);
            abs_diff_2p[i+4] = hls::abs(diff_2p[i+4]);
            diff_2p_gt_thr[i] = (abs_diff_2p[i] > regs.dpc_threshold);
            diff_2p_gt_thr[i+4] = (abs_diff_2p[i+4] > regs.dpc_threshold);
        }
        cond2_met = &diff_2p_gt_thr;
    }
    
    // do dpc when cond1 && cond2 both valid
    if (cond1_met && cond2_met) {
        ap_int<DATA_WIDTH + 2> dv = hls::abs(-(ap_int<DATA_WIDTH + 2>)pixel_window[1] + (ap_int<DATA_WIDTH + 2>)pixel_window[4]<<1 - (ap_int<DATA_WIDTH + 2>)pixel_window[7]);
        ap_int<DATA_WIDTH + 2> dh = hls::abs(-(ap_int<DATA_WIDTH + 2>)pixel_window[3] + (ap_int<DATA_WIDTH + 2>)pixel_window[4]<<1 - (ap_int<DATA_WIDTH + 2>)pixel_window[5]);
        ap_int<DATA_WIDTH + 2> ddr = hls::abs(-(ap_int<DATA_WIDTH + 2>)pixel_window[0] + (ap_int<DATA_WIDTH + 2>)pixel_window[4]<<1 - (ap_int<DATA_WIDTH + 2>)pixel_window[8]);
        ap_int<DATA_WIDTH + 2> ddl = hls::abs(-(ap_int<DATA_WIDTH + 2>)pixel_window[2] + (ap_int<DATA_WIDTH + 2>)pixel_window[4]<<1 - (ap_int<DATA_WIDTH + 2>)pixel_window[6]);
        
        // find min grad
        ap_int<DATA_WIDTH + 2> min_grad;
        ap_int<DATA_WIDTH + 2> min_grad_stg_1[2];
        ap_int<DATA_WIDTH + 2> min_grad_stg_2[1];
        ap_int<1> min_grad_stg_1_flag[2];
        ap_int<1> min_grad_stg_2_flag[1];
        min_grad_stg_1_flag[0] = dv >= dh ? 1 : 0;
        min_grad_stg_1_flag[1] = ddl >= ddr ? 1 : 0;
        min_grad_stg_2_flag[0] = min_grad_stg_1[0] >= min_grad_stg_1[1] ? 1 : 0;
        min_grad_stg_1[0] = min_grad_stg_1_flag[0] ? dv : dh;
        min_grad_stg_1[1] = min_grad_stg_1_flag[1] ? ddl : ddr;
        min_grad_stg_2[0] = min_grad_stg_2_flag[0] ? min_grad_stg_1[1] : min_grad_stg_1[0];
        min_grad = min_grad_stg_2[0];
        
        // correction in min grad
        if (min_grad_stg_2_flag[0]) {
            if (min_grad_stg_1_flag[0]) {
                // correction in vertical
                ap_uint<DATA_WIDTH> new_p0 = (pixel_window[1] + pixel_window[7]) / 2;
                pixel_out = new_p0;
            } else {
                // correction in horizontal
                ap_uint<DATA_WIDTH> new_p0 = (pixel_window[3] + pixel_window[5]) / 2;
                pixel_out = new_p0;
            }
        } else {
            if (min_grad_stg_1_flag[1]) {
                // correction in left diagonal
                ap_uint<DATA_WIDTH> new_p0 = (pixel_window[0] + pixel_window[8]) / 2;
                pixel_out = new_p0;
            } else {
                // correction in right diagonal
                ap_uint<DATA_WIDTH> new_p0 = (pixel_window[2] + pixel_window[6]) / 2;
                pixel_out = new_p0;
            }
        }
    }
}

// HlsDpc类的process方法实现
void HlsDpc::Process(
    hls::stream<axis_pixel_t>& input_stream,
    hls::stream<axis_pixel_t>& output_stream,
    const HlsDpcRegisterInfo& regs
) {
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=regs bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    #pragma HLS RESOURCE variable=hls_dpc_linebuffer core=RAM_2P
    
    ap_uint<16> cnt_y = 0;
    ap_uint<16> cnt_x = 0;
    axis_pixel_t input_data_pkt;
    axis_pixel_t output_data_pkt;
    
    // 如果DPC未启用，直接透传数据
    if (!regs.dpc_enable) {
        for(cnt_y = 0; cnt_y < regs.image_height; cnt_y++) {
            for(cnt_x = 0; cnt_x < regs.image_width; cnt_x++) {
                #pragma HLS PIPELINE II=1
                axis_pixel_t input_data_pkt = input_stream.read();
                output_stream.write(input_data_pkt);
            }
        }
        return;
    }

    // visual domain window
    ap_uint<DATA_WIDTH> pixel_window[3][5] = {0};
    ap_uint<DATA_WIDTH> pixel_win_remap[3][5] = {};
    ap_uint<DATA_WIDTH> pixel_process_window[9] = {};
    #pragma HLS ARRAY_PARTITION variable=pixel_window complete dim=2
    
    // 初始化
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            pixel_window[i][j] = 0;
        }
    }
    
    // 处理图像
    ap_uint<DATA_WIDTH> input_pixel_data;
    ap_uint<DATA_WIDTH> last_input_pixel_data;
    for (cnt_y = 0; cnt_y < regs.image_height+2; cnt_y++) {
        for (cnt_x = 0; cnt_x < regs.image_width+2; cnt_x++) {
            #pragma HLS PIPELINE II=1
            
            // input data update
            if (cnt_y < regs.image_height && cnt_x < regs.image_width) {
                
                // current data read
                last_input_pixel_data = input_pixel_data;
                input_pixel_data = input_stream.read().data;

                // linebuffer write
                if (cnt_x[0]==1) {
                    hls_dpc_linebuffer[cnt_y%5][cnt_x>>1] = input_pixel_data.concat(last_input_pixel_data);
                }
                
                #if 0
                    cout << "input_pixel_data debug" << endl;
                    cout << "(" << cnt_y << ", " << cnt_x << ")" << endl;
                    cout << input_pixel_data << " & " << last_input_pixel_data << endl;
                    cout << "{" << hls_dpc_linebuffer[cnt_y%5][cnt_x>>1].range(15, 8) << ", " << hls_dpc_linebuffer[cnt_y%5][cnt_x>>1].range(7, 0) << "}" << endl;
                #endif

            }
            
            // update 3x5 visual domain window
            
            // previous data update,
            ap_uint<16> dpc_lbuf_raddr_x[2];
            ap_uint<16> dpc_lbuf_raddr_y[2];
            ap_uint<DATA_WIDTH*2> dpc_lbuf_rdata_r[2];
            for (int y=0; y<2; y++) {
                if (cnt_x[0]==0) {
                    dpc_lbuf_raddr_x[y] = clip(cnt_x>>1, 0, (((regs.image_width+1)>>1)-1));
                    dpc_lbuf_raddr_y[y] = clip(cnt_y-4+2*y, 0, regs.image_height-1)%5;
                    dpc_lbuf_rdata_r[y] = hls_dpc_linebuffer[dpc_lbuf_raddr_y[y]][dpc_lbuf_raddr_x[y]];
                    if (cnt_x == regs.image_width-1) {
                        pixel_window[y][cnt_x%5] = dpc_lbuf_rdata_r[y](2*DATA_WIDTH-1, DATA_WIDTH);
                    } else {
                        pixel_window[y][cnt_x%5] = dpc_lbuf_rdata_r[y](DATA_WIDTH-1, 0);
                    }
                } else {
                    pixel_window[y][cnt_x%5] = dpc_lbuf_rdata_r[y](2*DATA_WIDTH-1, DATA_WIDTH);
                }
            }
            
            // current data update
            pixel_window[2][cnt_x%5] = input_pixel_data;
            
            #if 1
                printf("############### pixel_window debug\n");
                printf("coord = (%4x, %4x)\n", cnt_y.to_int(), cnt_x.to_int());
                for (int y=0; y<2; y++) {
                    printf("### cnt index = %4x, %4x\n", (cnt_x>>1).to_int(), (((regs.image_width+1)>>1)-1).to_int());
                    printf("### lbuf index = %4x, %4x\n", dpc_lbuf_raddr_y[y].to_int(), dpc_lbuf_raddr_x[y].to_int());
                    printf("### rdata = %4x %4x\n", dpc_lbuf_rdata_r[y].range(2*DATA_WIDTH-1, DATA_WIDTH).to_int(), dpc_lbuf_rdata_r[y].range(DATA_WIDTH-1, 0).to_int());
                }
                for (int y=0; y<3; y++) {
                    printf("%4x, %4x, %4x, %4x, %4x\n", pixel_window[y][(cnt_x-4)%5].to_int(), pixel_window[y][(cnt_x-3)%5].to_int(), pixel_window[y][(cnt_x-2)%5].to_int(), pixel_window[y][(cnt_x-1)%5].to_int(), pixel_window[y][(cnt_x-0)%5].to_int());
                }
            #endif
            
            // dpc proc
            if (cnt_y > 1 && cnt_x > 1) {

                // window data rearrange
                for (int y=0; y<3; y++) {
                    for (int x=0; x<3; x++) {
                        pixel_win_remap[y][x] = pixel_window[y][clip(cnt_x-4+2*x, 0, regs.image_width-1)%5];
                    }
                }
            
                #if 0
                    cout << "pixel_win_remap debug" << endl;
                    cout << "(" << cnt_y << ", " << cnt_x << ")" << endl;
                    cout << pixel_win_remap[0][0] << " " << pixel_win_remap[0][1] << " " << pixel_win_remap[0][2] << endl;
                    cout << pixel_win_remap[1][0] << " " << pixel_win_remap[1][1] << " " << pixel_win_remap[1][2] << endl;
                    cout << pixel_win_remap[2][0] << " " << pixel_win_remap[2][1] << " " << pixel_win_remap[2][2] << endl;
                #endif
                
                // pixel process
                for (int y=0; y<3; y++) {
                    for (int x=0; x<3; x++) {
                        pixel_process_window[y*3+x] = pixel_win_remap[y][x];
                    }
                }

                ap_uint<DATA_WIDTH> pixel_output;
                axis_pixel_t axis_pixel_t_output;
                ProcessDpc(pixel_process_window, pixel_output, regs);
                if (cnt_x==regs.image_width-1) {
                    axis_pixel_t_output.data = pixel_output;
                    axis_pixel_t_output.keep = 0x03;
                    axis_pixel_t_output.last = 1;
                    output_stream.write(axis_pixel_t_output);
                } else {
                    axis_pixel_t_output.data = pixel_output;
                    axis_pixel_t_output.keep = 0x03;
                    axis_pixel_t_output.last = 0;
                    output_stream.write(axis_pixel_t_output);
                }

            }

        }
    }
}