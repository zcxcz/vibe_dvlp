#ifndef ALG_CROP_H
#define ALG_CROP_H

// std
#include <vector>
#include <cstdint>
#include <random>
#include <fstream>
#include <iostream>
#include <algorithm>

// tool
#include "print_function.h"
#include "parse_json_function.h"
#include "alg_info.h"

// def
#define ALG_CROP_SECTION "[AlgCrop]"


template <typename ALG_INPUT_DATA_TYPE, typename ALG_OUTPUT_DATA_TYPE>
class AlgCrop {
public:
    void run(
            const std::vector<ALG_INPUT_DATA_TYPE>& input_image,
            std::vector<ALG_OUTPUT_DATA_TYPE>& output_image,
            const AlgRegisterSection& alg_register_section
        ) {
            
            if (!alg_register_section.reg_crop_enable) {
                output_image = input_image;
                return;
            }
            
            int expected_input_size = alg_register_section.reg_image_width * alg_register_section.reg_image_height;
            if (input_image.size() != expected_input_size) {
                MAIN_ERROR_1("Error: Input data size mismatch");
                return;
            }

            assert(input_image.size() == expected_input_size);
            
            if (alg_register_section.reg_crop_start_x < 0 || 
                alg_register_section.reg_crop_start_y < 0 || 
                alg_register_section.reg_crop_end_x < 0 || 
                alg_register_section.reg_crop_end_y < 0) {
                MAIN_ERROR_1("Error: Negative coordinates are not allowed");
                return;
            }
            
            if (alg_register_section.reg_crop_start_x > alg_register_section.reg_crop_end_x || 
                alg_register_section.reg_crop_start_y > alg_register_section.reg_crop_end_y) {
                MAIN_ERROR_1("Error: Start coordinates must be less than or equal to end coordinates");
                return;
            }
            
            if (alg_register_section.reg_crop_start_x >= alg_register_section.reg_image_width || 
                alg_register_section.reg_crop_end_x >= alg_register_section.reg_image_width || 
                alg_register_section.reg_crop_start_y >= alg_register_section.reg_image_height || 
                alg_register_section.reg_crop_end_y >= alg_register_section.reg_image_height) {
                MAIN_ERROR_1("Error: Crop coordinates exceed image dimensions");
                return;
            }
        
        int crop_width = alg_register_section.reg_crop_end_x - alg_register_section.reg_crop_start_x + 1;
        int crop_height = alg_register_section.reg_crop_end_y - alg_register_section.reg_crop_start_y + 1;
        
        if (crop_width <= 0 || crop_height <= 0) {
            MAIN_ERROR_1("Error: Invalid crop dimensions");
            return;
        }
            
        if (crop_width > alg_register_section.reg_image_width || crop_height > alg_register_section.reg_image_height) {
            MAIN_ERROR_1("Error: Crop dimensions exceed image dimensions");
            return;
        }
        
        std::vector<ALG_OUTPUT_DATA_TYPE> cropped_image(crop_width * crop_height);
        
        for (int y = alg_register_section.reg_crop_start_y; y <= alg_register_section.reg_crop_end_y; ++y) {
            for (int x = alg_register_section.reg_crop_start_x; x <= alg_register_section.reg_crop_end_x; ++x) {
                int src_idx = y * alg_register_section.reg_image_width + x;
                int dst_idx = (y - alg_register_section.reg_crop_start_y) * crop_width + (x - alg_register_section.reg_crop_start_x);
                if (src_idx < input_image.size()) {
                    cropped_image[dst_idx] = input_image[src_idx];
                }
            }
        }
        
        output_image = cropped_image;

    }

};

#endif // ALG_CROP_H