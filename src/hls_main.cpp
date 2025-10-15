// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

// tool
#include "json.hpp"
#include "print_function.h"
#include "vector_function.h"
#include "parse_json_function.h"
#include "parse_csv_function.h"

// hls
#include <ap_int.h>
#include <hls_stream.h>

// ip
#include "hls_top.h"

// using
using json = nlohmann::json;
using namespace std;

// def
// #define ALG_MAIN_SECTION "[AlgMain]"
#define ALG_INPUT_DATA_TYPE uint16_t
#define ALG_OUTPUT_DATA_TYPE uint16_t
#define HLS_INPUT_DATA_BITWIDTH     8
#define HLS_OUTPUT_DATA_BITWIDTH    8


int main(const int argc, const char *argv[]) {
    // try {
        // 读取JSON配置 - 兼容不同构建目录
    string config_path = "/home/sheldon/hls_project/vibe_crop/src/vibe.json";
    ifstream f(config_path);
    if (!f.is_open()) {
        MAIN_INFO_1("Cannot open vibe.json configuration file");
    }
    MAIN_INFO_1("vibe.json configuration file path: " + config_path);
    
    json data = json::parse(f);
    f.close();
    MAIN_INFO_1("vibe.json configuration file parse follow...");
    MAIN_INFO_1(data.dump(2));
    
    // json loading
    MAIN_INFO_1("object: image_section parse follow...");
    ImageSection image_section = data["image_info"].get<ImageSection>();
    MAIN_INFO_1("object: output_section parse follow...");
    OutputSection output_section = data["output_info"].get<OutputSection>();
    // MAIN_INFO_1("object: register_section parse follow...");
    // RegisterSection register_section = data["register_info"].get<RegisterSection>();

    MAIN_INFO_1("object: image_section parse follow...");
    image_section.print_values();
    MAIN_INFO_1("object: output_section parse follow...");
    output_section.print_values();
    // MAIN_INFO_1("object: register_section parse follow...");
    // register_section.print_values();
    // register_section.randomize_check();

    // csv loading
    MAIN_INFO_1("object: register_section parse follow...");
    RegisterSection register_section = data["register_info"].get<RegisterSection>();
    
    int width = register_section.reg_map["reg_image_width"].reg_initial_value[0];
    int height = register_section.reg_map["reg_image_height"].reg_initial_value[0];
    MAIN_INFO_1("image width: " + to_string(width));
    MAIN_INFO_1("image height: " + to_string(height));

    // src image load
    string source_image_path;
    vector<ap_uint<HLS_INPUT_DATA_BITWIDTH>> input_image;
    vector<ap_uint<HLS_OUTPUT_DATA_BITWIDTH>> output_image;
    if (image_section.generate_random_image) {
        source_image_path = image_section.random_image_path;
    } else {
        source_image_path = image_section.image_path;
    }
    MAIN_INFO_1("loading image: " + source_image_path);
    input_image = vector_read_from_file<ap_uint<HLS_INPUT_DATA_BITWIDTH>>(source_image_path);
    if (input_image.empty()) {
        MAIN_ERROR_1("Cannot load image: " + source_image_path);
    }

    // hls_top run
    MAIN_INFO_1("hls_top run...");
    HlsTop<ALG_INPUT_DATA_TYPE, ALG_OUTPUT_DATA_TYPE, HLS_INPUT_DATA_BITWIDTH, HLS_OUTPUT_DATA_BITWIDTH> hls_top;
    hls_top.run(register_section, image_section, output_section);
    
    return 0;
// }
}