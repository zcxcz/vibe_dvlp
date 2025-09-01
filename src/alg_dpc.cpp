#include "alg_dpc.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>

// 辅助函数：获取带镜像边界的像素值
alg_pixel_t get_mirrored_pixel(const std::vector<alg_pixel_t>& img, int width, int height, int x, int y) {
    // 镜像边界处理
    x = std::max(0, std::min(width - 1, x));
    y = std::max(0, std::min(height - 1, y));
    return img[y * width + x];
}

std::vector<alg_pixel_t> AlgDpc::process_image(
    const std::vector<alg_pixel_t>& input_image,
    int width, int height,
    bool enable,
    int threshold) {
        
    if (!enable) {
        return input_image;
    }
    
    // 验证输入数据量与预期是否匹配
    int expected_input_size = width * height;
    if (input_image.size() != expected_input_size) {
        std::cerr << "Error: Input data size mismatch. Expected: " << expected_input_size 
                  << " pixels, Actual: " << input_image.size() << " pixels" << std::endl;
        return {};
    }
    
    // 初始化输出数据为输入数据的副本
    std::vector<alg_pixel_t> output_image = input_image;
    
    // 遍历整幅图像
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int32_t p0 = input_image[y * width + x];

            // --- 坏点检测 ---
            // 条件1: 中心像素值是否在其邻域的最大/最小值范围之外
            // 使用5x5窗口但只考虑特定位置的像素（与Python版本的window对应）
            alg_pixel_t min_neighbor = 65535; // 假设alg_pixel_t是16位
            alg_pixel_t max_neighbor = 0;

            // 5x5窗口中的特定位置（对应Python版本的window）
            int window_positions[8][2] = {
                {-2, -2}, {-2, 0}, {-2, 2},
                {0, -2},          {0, 2},
                {2, -2},  {2, 0}, {2, 2}
            };

            for (int i = 0; i < 8; ++i) {
                int nx = x + window_positions[i][0];
                int ny = y + window_positions[i][1];
                alg_pixel_t neighbor = get_mirrored_pixel(input_image, width, height, nx, ny);
                min_neighbor = std::min(min_neighbor, neighbor);
                max_neighbor = std::max(max_neighbor, neighbor);
            }

            #if 1
                int32_t p00 = get_mirrored_pixel(input_image, width, height, x-2, y-2);
                int32_t p01 = get_mirrored_pixel(input_image, width, height, x, y-2);
                int32_t p02 = get_mirrored_pixel(input_image, width, height, x+2, y-2);
                int32_t p10 = get_mirrored_pixel(input_image, width, height, x-2, y);
                int32_t p11 = get_mirrored_pixel(input_image, width, height, x, y);
                int32_t p12 = get_mirrored_pixel(input_image, width, height, x+2, y);
                int32_t p20 = get_mirrored_pixel(input_image, width, height, x-2, y+2);
                int32_t p21 = get_mirrored_pixel(input_image, width, height, x, y+2);
                int32_t p22 = get_mirrored_pixel(input_image, width, height, x+2, y+2);
                printf("pixel win remap debug\n");
                printf("coord = (%4x, %4x)\n", x, y);
                printf("%4x, %4x, %4x\n", p00, p01, p02);
                printf("%4x, %4x, %4x\n", p10, p11, p12);
                printf("%4x, %4x, %4x\n", p20, p21, p22);
            #endif

            bool cond1_met = (p0 < min_neighbor) || (p0 > max_neighbor);

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
                    alg_pixel_t neighbor = get_mirrored_pixel(input_image, width, height, nx, ny);
                    if (std::abs(p0 - static_cast<int32_t>(neighbor)) <= threshold) {
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
                int32_t p_up = get_mirrored_pixel(input_image, width, height, x, y-2);
                int32_t p_down = get_mirrored_pixel(input_image, width, height, x, y+2);
                int32_t dv = std::abs(-p_up + 2*p0 - p_down);

                // 水平梯度
                int32_t p_left = get_mirrored_pixel(input_image, width, height, x-2, y);
                int32_t p_right = get_mirrored_pixel(input_image, width, height, x+2, y);
                int32_t dh = std::abs(-p_left + 2*p0 - p_right);

                // 左对角线梯度 (左上-右下)
                int32_t p_ul = get_mirrored_pixel(input_image, width, height, x-2, y-2);
                int32_t p_dr = get_mirrored_pixel(input_image, width, height, x+2, y+2);
                int32_t ddl = std::abs(-p_ul + 2*p0 - p_dr);

                // 右对角线梯度 (右上-左下)
                int32_t p_ur = get_mirrored_pixel(input_image, width, height, x+2, y-2);
                int32_t p_dl = get_mirrored_pixel(input_image, width, height, x-2, y+2);
                int32_t ddr = std::abs(-p_ur + 2*p0 - p_dl);

                // 找到最小梯度
                int32_t min_grad = std::min({dv, dh, ddl, ddr});

                // 沿最小梯度方向进行插值
                int32_t new_p0;
                if (min_grad == dv) {
                    // 垂直方向
                    new_p0 = (get_mirrored_pixel(input_image, width, height, x, y-1) + 
                             get_mirrored_pixel(input_image, width, height, x, y+1)) / 2;
                } else if (min_grad == dh) {
                    // 水平方向
                    new_p0 = (get_mirrored_pixel(input_image, width, height, x-1, y) + 
                             get_mirrored_pixel(input_image, width, height, x+1, y)) / 2;
                } else if (min_grad == ddl) {
                    // 左对角线方向
                    new_p0 = (get_mirrored_pixel(input_image, width, height, x-1, y-1) + 
                             get_mirrored_pixel(input_image, width, height, x+1, y+1)) / 2;
                } else { // min_grad == ddr
                    // 右对角线方向
                    new_p0 = (get_mirrored_pixel(input_image, width, height, x+1, y-1) + 
                             get_mirrored_pixel(input_image, width, height, x-1, y+1)) / 2;
                }

                // 使用校正后的值更新输出数据
                output_image[y * width + x] = static_cast<alg_pixel_t>(new_p0);
            }
        }
    }

    return output_image;
}


