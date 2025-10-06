// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// tool
#include "json.hpp"
#include "print_function.h"
#include "vector_function.h"
#include "parse_json_function.h"

// ip
#include "alg_top.h"
#include <algorithm>

using json = nlohmann::json;
using namespace std;

// def
#define ALG_MAIN_SECTION "[AlgMain]"
#define ALG_INPUT_DATA_TYPE uint16_t
#define ALG_OUTPUT_DATA_TYPE uint16_t

int main(const int argc, const char *argv[]) {
    // json config loading
    string config_path = "/home/sheldon/hls_project/vibe_crop/src/vibe.json";
    ifstream f(config_path);
    if (!f.is_open()) {
        MAIN_ERROR_1("Cannot open vibe.json configuration file");
    }
    MAIN_INFO_1("vibe.json configuration file path: " + config_path);
    
    json data = json::parse(f);
    f.close();
    MAIN_INFO_1("vibe.json configuration file parse follow...");
    MAIN_INFO_1(data.dump(2));
    
    // object loading
    MAIN_INFO_1("object: image_section print follow...");
    ImageSection image_section = data["image_info"].get<ImageSection>();
    MAIN_INFO_1("object: register_section print follow...");
    RegisterSection register_section = data["register_info"].get<RegisterSection>();
    MAIN_INFO_1("object: output_section print follow...");
    OutputSection output_section = data["output_info"].get<OutputSection>();

    // object print
    MAIN_INFO_1("object: image_section print follow...");
    image_section.print_values();
    MAIN_INFO_1("object: register_section print follow...");
    register_section.print_values();
    MAIN_INFO_1("object: output_section print follow...");
    output_section.print_values();

    int width = register_section.reg_image_width.reg_initial_value[0];
    int height = register_section.reg_image_height.reg_initial_value[0];
    MAIN_INFO_1("image width: " + to_string(width));
    MAIN_INFO_1("image height: " + to_string(height));
    
    // src image loading
    string source_image_path;
    vector<ALG_INPUT_DATA_TYPE> input_image;
    vector<ALG_OUTPUT_DATA_TYPE> output_image;
    if (image_section.generate_random_image) {
        source_image_path = image_section.random_image_path;
    } else {
        source_image_path = image_section.image_path;
    }
    MAIN_INFO_1("loading image: " + source_image_path);
    input_image = vector_read_from_file<ALG_INPUT_DATA_TYPE>(source_image_path);
    if (input_image.empty()) {
        MAIN_ERROR_1("Cannot load image: " + source_image_path);
    }

    // alg_top run
    MAIN_INFO_1("alg_top run...");
    AlgTop<ALG_INPUT_DATA_TYPE, ALG_OUTPUT_DATA_TYPE> alg_top;
    alg_top.loadSection(register_section, image_section, output_section);
    alg_top.printSection();
    alg_top.run(input_image, output_image);
    
    return 0;
}