#include "alg_dpc.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>

std::vector<alg_pixel_t> AlgDpc::process_image(
    const std::vector<alg_pixel_t>& input_image,
    int width, int height,
    int threshold) {
    
    // 验证输入数据量与预期是否匹配
    int expected_input_size = width * height;
    if (input_image.size() != expected_input_size) {
        std::cerr << "Error: Input data size mismatch. Expected: " << expected_input_size 
                  << " pixels, Actual: " << input_image.size() << " pixels" << std::endl;
        return {};
    }
    
    // 初始化输出数据为输入数据的副本
    std::vector<alg_pixel_t> output_image = input_image;
    
    // 遍历图像的内部像素，跳过1像素的边缘
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            // 使用 int32_t 进行计算以防止溢出
            const int32_t p0 = input_image[y * width + x];

            // 获取 3x3 邻域像素
            const alg_pixel_t neighbors[8] = {
                input_image[(y - 1) * width + (x - 1)], // p1 (top-left)
                input_image[(y - 1) * width + x],       // p2 (top)
                input_image[(y - 1) * width + (x + 1)], // p3 (top-right)
                input_image[y * width + (x - 1)],       // p4 (left)
                input_image[y * width + (x + 1)],       // p5 (right)
                input_image[(y + 1) * width + (x - 1)], // p6 (bottom-left)
                input_image[(y + 1) * width + x],       // p7 (bottom)
                input_image[(y + 1) * width + (x + 1)]  // p8 (bottom-right)
            };

            // --- 坏点检测 ---

            // 条件1: 中心像素值是否在其邻域的最大/最小值范围之外
            alg_pixel_t min_neighbor = std::numeric_limits<alg_pixel_t>::max();
            alg_pixel_t max_neighbor = 0;
            for (int i = 0; i < 8; ++i) {
                if (neighbors[i] < min_neighbor) min_neighbor = neighbors[i];
                if (neighbors[i] > max_neighbor) max_neighbor = neighbors[i];
            }
            bool cond1_met = (p0 < min_neighbor) || (p0 > max_neighbor);

            // 条件2: 中心像素与所有8个邻居的差的绝对值是否都大于阈值
            bool cond2_met = true;
            if (cond1_met) { // 优化：仅当条件1满足时才检查条件2
                for (int i = 0; i < 8; ++i) {
                    if (std::abs(p0 - static_cast<int32_t>(neighbors[i])) <= threshold) {
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
                const int32_t p_ul = neighbors[0];
                const int32_t p_up = neighbors[1];
                const int32_t p_ur = neighbors[2];
                const int32_t p_left = neighbors[3];
                const int32_t p_right = neighbors[4];
                const int32_t p_dl = neighbors[5];
                const int32_t p_down = neighbors[6];
                const int32_t p_dr = neighbors[7];

                // 计算四个方向的梯度
                const int32_t dv = std::abs(2 * p0 - p_up - p_down);
                const int32_t dh = std::abs(2 * p0 - p_left - p_right);
                const int32_t ddl = std::abs(2 * p0 - p_ul - p_dr); // 左上-右下对角线
                const int32_t ddr = std::abs(2 * p0 - p_ur - p_dl); // 右上-左下对角线

                // 找到最小梯度
                const int32_t min_grad = std::min({dv, dh, ddl, ddr});

                // 沿最小梯度方向进行插值
                int32_t new_p0;
                if (min_grad == dv) {
                    new_p0 = (p_up + p_down) / 2;
                } else if (min_grad == dh) {
                    new_p0 = (p_left + p_right) / 2;
                } else if (min_grad == ddl) {
                    new_p0 = (p_ul + p_dr) / 2;
                } else { // min_grad == ddr
                    new_p0 = (p_ur + p_dl) / 2;
                }

                // 使用校正后的值更新输出数据
                output_image[y * width + x] = static_cast<alg_pixel_t>(new_p0);
            }
        }
    }

    return output_image;
}


