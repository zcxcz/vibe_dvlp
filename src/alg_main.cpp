// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// tool
#include "json.hpp"
#include "print_function.h"
#include "vector_function.h"
#include "image_info.h"

// ip
#include "alg_top.h"
#include <algorithm>

using json = nlohmann::json;
using namespace std;

// def
#define ALG_MAIN_SECTION "[AlgMain]"

int main(const int argc, const char *argv[]) {
    // json config loading
    string config_path = "/home/sheldon/hls_project/vibe_crop/src/vibe.json";
    ifstream f(config_path);
    if (!f.is_open()) {
        main_error(ALG_MAIN_SECTION, "Cannot open vibe.json configuration file");
    }
    main_info(ALG_MAIN_SECTION, "vibe.json configuration file path: " + config_path);
    
    json data = json::parse(f);
    f.close();
    main_info(ALG_MAIN_SECTION, "vibe.json configuration file parse follow:");
    main_info(ALG_MAIN_SECTION, data.dump(2));
    
    // object loading
    main_info(ALG_MAIN_SECTION, "object: image_section print follow:");
    ImageSection image_section = data["image_info"].get<ImageSection>();
    main_info(ALG_MAIN_SECTION, "object: register_section print follow:");
    RegisterSection register_section = data["register_info"].get<RegisterSection>();
    main_info(ALG_MAIN_SECTION, "object: output_section print follow:");
    OutputSection output_section = data["output_info"].get<OutputSection>();

    // object print
    main_info(ALG_MAIN_SECTION, "object: image_section print follow:");
    image_section.print_values();
    main_info(ALG_MAIN_SECTION, "object: register_section print follow:");
    register_section.print_values();
    main_info(ALG_MAIN_SECTION, "object: output_section print follow:");
    output_section.print_values();

    int width = register_section.reg_image_width.reg_initial_value[0];
    int height = register_section.reg_image_height.reg_initial_value[0];
    main_info(ALG_MAIN_SECTION, "image width: " + to_string(width));
    main_info(ALG_MAIN_SECTION, "image height: " + to_string(height));
    
    // src image loading
    string source_image_path;
    vector<uint16_t> input_image;   
    bool image_load = false;
    if (image_section.generate_random_image) {
        source_image_path = image_section.random_image_path;
    } else {
        source_image_path = image_section.image_path;
    }
    main_info(ALG_MAIN_SECTION, "loading image: " + source_image_path);
    input_image = vector_read_from_file<uint16_t>(source_image_path);
    if (input_image.empty()) {
        main_error(ALG_MAIN_SECTION, "Cannot load image: " + source_image_path);
    }
        
    // 算法模型处理
    AlgTop alg_top;
    alg_top.loadSection(register_section, image_section, output_section);
    alg_top.printSection();
    // alg_top.run();
    
    return 0;
}