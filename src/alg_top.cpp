// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <string>

// tool
#include "image_info.h"
#include "print_function.h"
#include "alg_top.h"

// ip
#include "alg_crop.h"
#include "alg_dpc.h"

// using
using json = nlohmann::json;
using namespace std;

// def
#define ALG_TOP_SECTION "[AlgTop]"

AlgTop::AlgTop() {}
AlgTop::~AlgTop() {}


// 从JSON文件加载配置
void AlgTop::loadRegisterSection(const RegisterSection& register_section) {
    // register info
    main_info(ALG_TOP_SECTION, "Register Section loading...");
    alg_register_section.width = register_section.reg_image_width.reg_initial_value[0];
    alg_register_section.height = register_section.reg_image_height.reg_initial_value[0];
    alg_register_section.crop_start_x = register_section.reg_crop_start_x.reg_initial_value[0];
    alg_register_section.crop_start_y = register_section.reg_crop_start_y.reg_initial_value[0];
    alg_register_section.crop_end_x = register_section.reg_crop_end_x.reg_initial_value[0];
    alg_register_section.crop_end_y = register_section.reg_crop_end_y.reg_initial_value[0];
    alg_register_section.crop_enable = (register_section.reg_crop_enable.reg_initial_value[0] != 0);

    alg_register_section.dpc_enable = (register_section.reg_dpc_enable.reg_initial_value[0] != 0);
    alg_register_section.dpc_threshold = register_section.reg_dpc_threshold.reg_initial_value[0];
}

void AlgTop::loadImageSection(const ImageSection& image_section) {
    // image info
    main_info(ALG_TOP_SECTION, "Image Section loading...");
    alg_image_section.input_file = image_section.image_path;
    alg_image_section.random_image_path = image_section.random_image_path;
    alg_image_section.generate_random_image = image_section.generate_random_image;
}

void AlgTop::loadOutputSection(const OutputSection& output_section) {
    // image info
    main_info(ALG_TOP_SECTION, "Output Section loading...");
    alg_output_section.crop_output_file = output_section.alg_crop_output;
    alg_output_section.dpc_output_file = output_section.alg_dpc_output;
    alg_output_section.hls_crop_output_file = output_section.hls_crop_output;
    alg_output_section.hls_dpc_output_file = output_section.hls_dpc_output;
}

void AlgTop::loadSection(const RegisterSection& register_section, const ImageSection& image_section, const OutputSection& output_section) {
    loadRegisterSection(register_section);
    loadImageSection(image_section);
    loadOutputSection(output_section);
}

void AlgTop::printRegisterSection() {
    main_info(ALG_TOP_SECTION, "Register Section Configuration");
    cout << "Width: " << alg_register_section.width << endl;
    cout << "Height: " << alg_register_section.height << endl;
    cout << "Crop Enable: " << (alg_register_section.crop_enable ? "true" : "false") << endl;
    cout << "Crop Start X: " << alg_register_section.crop_start_x << endl;
    cout << "Crop Start Y: " << alg_register_section.crop_start_y << endl;
    cout << "Crop End X: " << alg_register_section.crop_end_x << endl;
    cout << "Crop End Y: " << alg_register_section.crop_end_y << endl;
    cout << "DPC Enable: " << (alg_register_section.dpc_enable ? "true" : "false") << endl;
    cout << "DPC Threshold: " << alg_register_section.dpc_threshold << endl;
}

void AlgTop::printImageSection() {
    main_info(ALG_TOP_SECTION, "Image Section Configuration");
    cout << "Input File: " << alg_image_section.input_file << endl;
    cout << "Random Image Path: " << alg_image_section.random_image_path << endl;
    cout << "Generate Random Image: " << (alg_image_section.generate_random_image ? "true" : "false") << endl;
}

void AlgTop::printOutputSection() {
    main_info(ALG_TOP_SECTION, "Output Section Configuration");
    cout << "Crop Output File: " << alg_output_section.crop_output_file << endl;
    cout << "DPC Output File: " << alg_output_section.dpc_output_file << endl;
    cout << "HLS Crop Output File: " << alg_output_section.hls_crop_output_file << endl;
    cout << "HLS DPC Output File: " << alg_output_section.hls_dpc_output_file << endl;
}

void AlgTop::run(const RegisterSection& config) {
    // 这里需要根据实际的AlgCrop和AlgDpc的run函数参数来调整
    // 暂时留空，先让编译通过
}

void AlgTop::printSection() {
    printRegisterSection();
    printImageSection();
    printOutputSection();
}
