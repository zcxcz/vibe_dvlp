// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>

// tool
#include "json.hpp"
#include "print_function.h"
#include "vector_function.h"
#include "image_info.h"

// def
#define GEN_IMAGE_MAIN_SECTION "[gen_image_main]"

// using
using json = nlohmann::json;
using namespace std;

int main(const int argc, const char *argv[]) {
    string config_path = "/home/sheldon/hls_project/vibe_crop/src/vibe.json";
    ifstream f(config_path);
    if (!f.is_open()) {
        main_error(GEN_IMAGE_MAIN_SECTION, "Cannot open vibe.json configuration file");
    }
    main_info(GEN_IMAGE_MAIN_SECTION, "vibe.json configuration file path: " + config_path);
    
    json data = json::parse(f);
    f.close();
    
    ImageSection image_section = data["image_info"].get<ImageSection>();
    RegisterSection register_section = data["register_info"].get<RegisterSection>();
    main_info(GEN_IMAGE_MAIN_SECTION, "object: image_info print follow:");
    image_section.print_values();
    main_info(GEN_IMAGE_MAIN_SECTION, "object: register_info print follow:");
    register_section.print_values();
    
    int width = register_section.reg_image_width.reg_initial_value[0];
    int height = register_section.reg_image_height.reg_initial_value[0];

    vector<uint16_t> input_image;   

    // 生成或读取输入图像 - 多路径兼容处理

    if (image_section.generate_random_image == 1) {

        main_info(GEN_IMAGE_MAIN_SECTION, "Random image generate enabled, width:height= " + to_string(width) + "x" + to_string(height));
        // 生成随机图像
        input_image.resize(width * height);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 255);
        for (auto& pixel : input_image) {
            pixel = static_cast<uint16_t>(distrib(gen));
        }

        string save_path = image_section.random_image_path;
        if (vector_write_to_file(save_path, input_image, width, height)) {
            main_info(GEN_IMAGE_MAIN_SECTION, "Random image write to file success: " + save_path);
        } else {
            main_info(GEN_IMAGE_MAIN_SECTION, "Random image write to file fail: " + save_path);
        }

    }
    
    return 0;
}