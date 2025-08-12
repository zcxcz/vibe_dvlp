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
    // 生成随机图像数据
    static std::vector<alg_pixel_t> generate_random_image(int width, int height, int max_value);
    
    // 图像裁剪算法
    static std::vector<alg_pixel_t> crop_image(
        const std::vector<alg_pixel_t>& input_image,
        int width, int height,
        int start_x, int start_y,
        int end_x, int end_y,
        bool enable
    );
    
    // 从文件读取图像数据
    static std::vector<alg_pixel_t> read_image_from_file(const std::string& filename);
    
    // 将图像数据写入文件
    static bool write_image_to_file(const std::string& filename, const std::vector<alg_pixel_t>& data);
};

#endif // ALG_CROP_H