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
    string register_table_csv_path = "../src/register_table.csv";
    string image_config_json_path = "../src/image_config.json";

    MAIN_INFO_1("image_config json parse");
    MAIN_INFO_1("image_config.image_section parse");
    ImageSection image_section = LoadImageConfigJsonImageSection(image_config_json_path);
    image_section.print_values();
    MAIN_INFO_1("image_config.output_section parse");
    OutputSection output_section = LoadImageConfigJsonOutputSection(image_config_json_path);
    output_section.print_values();
    MAIN_INFO_1("register_table.register_section parse");
    RegisterSection register_section = LoadCSVFile(register_table_csv_path);
    register_section.print_values();
    
    int width = register_section.reg_map["reg_image_width"];
    int height = register_section.reg_map["reg_image_height"];
    MAIN_INFO_1("image width: " + to_string(width));
    MAIN_INFO_1("image height: " + to_string(height));

    exit();
    // hls_top run
    MAIN_INFO_1("hls_top run...");
    HlsTop<ALG_INPUT_DATA_TYPE, ALG_OUTPUT_DATA_TYPE, HLS_INPUT_DATA_BITWIDTH, HLS_OUTPUT_DATA_BITWIDTH> hls_top;
    hls_top.run(register_section, image_section, output_section);
    
    return 0;
// }
}