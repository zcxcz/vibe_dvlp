#ifndef ALG_DPC_H
#define ALG_DPC_H

// std
#include <vector>
#include <cstdint>

// tool
#include "print_function.h"
#include "parse_json_function.h"
#include "alg_info.h"

// def
#define ALG_DPC_SECTION "[AlgDpc]"

// 算法模型使用的数据类型
using alg_pixel_t = uint16_t;

template <typename ALG_INPUT_DATA_TYPE, typename ALG_OUTPUT_DATA_TYPE>
class AlgDpc {
public:
    void run(
            const std::vector<ALG_INPUT_DATA_TYPE>& input_image,
            std::vector<ALG_OUTPUT_DATA_TYPE>& output_image,
            const AlgRegisterSection& alg_register_section
        ) {
            
            if (!alg_register_section.reg_dpc_enable) {
                output_image = input_image;
                return;
            }
            
            int expected_input_size = alg_register_section.reg_image_width * alg_register_section.reg_image_height;
            if (input_image.size() != expected_input_size) {
                MAIN_ERROR_1("Error: Input data size mismatch");
                return;
            }

            assert(input_image.size() == expected_input_size);
            
            // 转换输入图像到alg_pixel_t类型
            std::vector<alg_pixel_t> input_converted(input_image.begin(), input_image.end());
            
            // 调用处理函数
            std::vector<alg_pixel_t> result = process_image(
                input_converted,
                alg_register_section.reg_image_width,
                alg_register_section.reg_image_height,
                alg_register_section.reg_dpc_enable,
                alg_register_section.reg_dpc_threshold
            );
            
            // 转换结果到输出类型
            output_image.assign(result.begin(), result.end());
        }
    
    static std::vector<alg_pixel_t> process_image(
        const std::vector<alg_pixel_t>& input_image,
        int width, int height,
        bool enable,
        int threshold
    );
};

#endif // ALG_DPC_H
