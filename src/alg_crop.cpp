#include "alg_crop.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <fstream>

std::vector<alg_pixel_t> AlgCrop::crop_image(
    const std::vector<alg_pixel_t>& input_image,
    int width, int height,
    int start_x, int start_y,
    int end_x, int end_y,
    bool enable) {
    
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
    
    // 验证寄存器配置的有效性
    if (start_x < 0 || start_y < 0 || end_x < 0 || end_y < 0) {
        std::cerr << "Error: Negative coordinates are not allowed" << std::endl;
        return {};
    }
    
    if (start_x > end_x || start_y > end_y) {
        std::cerr << "Error: Start coordinates must be less than or equal to end coordinates" << std::endl;
        return {};
    }
    
    if (start_x >= width || end_x >= width || start_y >= height || end_y >= height) {
        std::cerr << "Error: Crop coordinates exceed image dimensions" << std::endl;
        return {};
    }
    
    // 验证裁剪区域是否在图像范围内
    int crop_width = end_x - start_x + 1;
    int crop_height = end_y - start_y + 1;
    
    if (crop_width <= 0 || crop_height <= 0) {
        std::cerr << "Error: Invalid crop dimensions" << std::endl;
        return {};
    }
    
    if (crop_width > width || crop_height > height) {
        std::cerr << "Error: Crop dimensions exceed image dimensions" << std::endl;
        return {};
    }
    
    std::vector<alg_pixel_t> cropped_image(crop_width * crop_height);
    
    for (int y = start_y; y <= end_y; ++y) {
        for (int x = start_x; x <= end_x; ++x) {
            int src_idx = y * width + x;
            int dst_idx = (y - start_y) * crop_width + (x - start_x);
            if (src_idx < input_image.size()) {
                cropped_image[dst_idx] = input_image[src_idx];
            }
        }
    }
    
    return cropped_image;
}

// 生成随机图像
std::vector<alg_pixel_t> AlgCrop::generate_random_image(int width, int height, int max_value) {
    std::vector<alg_pixel_t> image(width * height);
    
    // 使用随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, max_value);
    
    // 填充随机值
    for (int i = 0; i < width * height; ++i) {
        image[i] = static_cast<alg_pixel_t>(dis(gen));
    }
    
    return image;
}

// 写入图像到文件
bool AlgCrop::write_image_to_file(const std::string& filename, const std::vector<alg_pixel_t>& data) {
    std::ofstream output_file(filename);
    if (!output_file) {
        std::cerr << "Cannot open output file: " << filename << std::endl;
        return false;
    }
    
    for (const auto& value : data) {
        output_file << static_cast<int>(value) << "\n";
    }
    
    output_file.close();
    return true;
}
