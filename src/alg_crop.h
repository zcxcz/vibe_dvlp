#ifndef ALG_CROP_H
#define ALG_CROP_H

#include <vector>
#include <cstdint>
#include <fstream>
#include <random>

// 算法模型使用的数据类型
using alg_pixel_t = uint16_t;

class AlgCrop {
public:
    // 图像裁剪算法
    void crop_image(
        const std::vector<alg_pixel_t>& input_image,
        std::vector<alg_pixel_t>& output_image,
        int width, int height,
        int start_x, int start_y,
        int end_x, int end_y,
        bool enable
    );
    
};

#endif // ALG_CROP_H