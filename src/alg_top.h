#ifndef ALG_TOP_H
#define ALG_TOP_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <string>

// tool
#include "parse_json_function.h"
#include "print_function.h"
#include "vector_function.h"

// ip
#include "alg_info.h"
#include "alg_crop.h"
// #include "alg_dpc.h"

// using
using json = nlohmann::json;
using namespace std;

// def
#define ALG_TOP_SECTION "AlgTop"


template <typename ALG_INPUT_DATA_TYPE, typename ALG_OUTPUT_DATA_TYPE>
class AlgTop {
public:
    AlgTop();
    ~AlgTop();
    
    // section object
    AlgRegisterSection alg_register_section;
    AlgImageSection alg_image_section;
    AlgOutputSection alg_output_section;

    // data object
    vector<ALG_INPUT_DATA_TYPE> alg_crop_input_image;
    vector<ALG_INPUT_DATA_TYPE> alg_crop_output_image;
    vector<ALG_OUTPUT_DATA_TYPE> alg_dpc_output_image;

    // ip object
    AlgCrop<ALG_INPUT_DATA_TYPE, ALG_INPUT_DATA_TYPE> alg_crop;
    // AlgDpc<ALG_INPUT_DATA_TYPE, ALG_OUTPUT_DATA_TYPE> alg_dpc;


    void loadRegisterSection(const RegisterSection& register_section) {
        // register info
        MAIN_INFO_1("Register Section loading...");
        alg_register_section.reg_image_width = register_section.reg_image_width.reg_initial_value[0];
        alg_register_section.reg_image_height = register_section.reg_image_height.reg_initial_value[0];
        alg_register_section.reg_crop_start_x = register_section.reg_crop_start_x.reg_initial_value[0];
        alg_register_section.reg_crop_start_y = register_section.reg_crop_start_y.reg_initial_value[0];
        alg_register_section.reg_crop_end_x = register_section.reg_crop_end_x.reg_initial_value[0];
        alg_register_section.reg_crop_end_y = register_section.reg_crop_end_y.reg_initial_value[0];
        alg_register_section.reg_crop_enable = (register_section.reg_crop_enable.reg_initial_value[0] != 0);

        alg_register_section.reg_dpc_enable = (register_section.reg_dpc_enable.reg_initial_value[0] != 0);
        alg_register_section.reg_dpc_threshold = register_section.reg_dpc_threshold.reg_initial_value[0];
    }

    void loadImageSection(const ImageSection& image_section) {
        // image info
        MAIN_INFO_1("Image Section loading...");
        alg_image_section.image_path = image_section.image_path;
        alg_image_section.random_image_path = image_section.random_image_path;
        alg_image_section.generate_random_image = image_section.generate_random_image;
    }

    void loadOutputSection(const OutputSection& output_section) {
        // image info
        MAIN_INFO_1("Output Section loading...");
        alg_output_section.alg_crop_output_path = output_section.alg_crop_output_path;
        alg_output_section.alg_dpc_output_path = output_section.alg_dpc_output_path;
        alg_output_section.hls_crop_output_path = output_section.hls_crop_output_path;
        alg_output_section.hls_dpc_output_path = output_section.hls_dpc_output_path;
    }

    void loadSection(const RegisterSection& register_section, const ImageSection& image_section, const OutputSection& output_section) {
        loadRegisterSection(register_section);
        loadImageSection(image_section);
        loadOutputSection(output_section);
    }

    
    void loadImage() {
        MAIN_INFO_1("Loading source image...");
        if (alg_image_section.generate_random_image) {
            alg_crop_input_image = vector_read_from_file<ALG_INPUT_DATA_TYPE>(alg_image_section.random_image_path);
        } else {
            alg_crop_input_image = vector_read_from_file<ALG_INPUT_DATA_TYPE>(alg_image_section.image_path);
        }
    }


    void printRegisterSection() {
        MAIN_INFO_1("Register Section Configuration");
        cout << "Width: " << alg_register_section.reg_image_width << endl;
        cout << "Height: " << alg_register_section.reg_image_height << endl;
        cout << "Crop Enable: " << (alg_register_section.reg_crop_enable ? "true" : "false") << endl;
        cout << "Crop Start X: " << alg_register_section.reg_crop_start_x << endl;
        cout << "Crop Start Y: " << alg_register_section.reg_crop_start_y << endl;
        cout << "Crop End X: " << alg_register_section.reg_crop_end_x << endl;
        cout << "Crop End Y: " << alg_register_section.reg_crop_end_y << endl;
        cout << "DPC Enable: " << (alg_register_section.reg_dpc_enable ? "true" : "false") << endl;
        cout << "DPC Threshold: " << alg_register_section.reg_dpc_threshold << endl;
    }

    void printImageSection() {
        MAIN_INFO_1("Image Section Configuration");
        cout << "Input File: " << alg_image_section.image_path << endl;
        cout << "Random Image Path: " << alg_image_section.random_image_path << endl;
        cout << "Generate Random Image: " << (alg_image_section.generate_random_image ? "true" : "false") << endl;
    }

    void printOutputSection() {
        MAIN_INFO_1("Output Section Configuration");
        cout << "Crop Output File: " << alg_output_section.alg_crop_output_path << endl;
        cout << "DPC Output File: " << alg_output_section.alg_dpc_output_path << endl;
        cout << "HLS Crop Output File: " << alg_output_section.hls_crop_output_path << endl;
        cout << "HLS DPC Output File: " << alg_output_section.hls_dpc_output_path << endl;
    }

    void printSection() {
        printRegisterSection();
        printImageSection();
        printOutputSection();
    }


    void run(const vector<ALG_INPUT_DATA_TYPE>& input_image, vector<ALG_OUTPUT_DATA_TYPE>& output_image) {
        MAIN_INFO_1("AlgTop run...");
        
        // src image loading...
        loadImage();
        
        // alg crop running...
        MAIN_INFO_1("Running crop algorithm...");
        alg_crop.run(alg_crop_input_image, alg_crop_output_image, alg_register_section);
        MAIN_INFO_1("Saving crop output to: " + alg_output_section.alg_crop_output_path);
        vector_write_to_file<ALG_OUTPUT_DATA_TYPE>(alg_output_section.alg_crop_output_path, alg_crop_output_image, 
                                                        alg_register_section.reg_image_width, alg_register_section.reg_image_height);
        
        MAIN_INFO_1("Algorithm execution completed");
    }


};


#endif // ALG_TOP_H