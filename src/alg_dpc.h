#ifndef ALG_DPC_H
#define ALG_DPC_H

#include <vector>
#include <cstdint>

// 算法模型使用的数据类型
using alg_pixel_t = uint16_t;

class AlgDpc {
public:
    
    /**
     * @brief 对图像执行动态坏点校正。
     * @param input_image 输入的图像数据（一维向量）。
     * @param width 图像宽度。
     * @param height 图像高度。
     * @param threshold DPC阈值。
     * @return std::vector<alg_pixel_t> 校正后的图像数据。
     */
    static std::vector<alg_pixel_t> process_image(
        const std::vector<alg_pixel_t>& input_image,
        int width, int height,
        int threshold
    );
};

#endif // ALG_DPC_H
