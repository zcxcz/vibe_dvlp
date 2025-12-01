#ifndef HLS_TOP_H
#define HLS_TOP_H

// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <string>

// tool
#include "json.hpp"
#include "parse_json_function.h"
#include "print_function.h"
#include "vector_function.h"

// ip
#include "hls_info.h"
#include "hls_crop.h"
// #include "hls_dpc.h"

// using
using json = nlohmann::json;
using namespace std;
using namespace hls;

// def
#define HLS_CROP_INPUT_DATA_BITWIDTH    HLS_INPUT_DATA_BITWIDTH
#define HLS_CROP_OUTPUT_DATA_BITWIDTH   HLS_OUTPUT_DATA_BITWIDTH
#define HLS_DPC_INPUT_DATA_BITWIDTH     HLS_INPUT_DATA_BITWIDTH
#define HLS_DPC_OUTPUT_DATA_BITWIDTH    HLS_OUTPUT_DATA_BITWIDTH


template <typename ALG_INPUT_DATA_TYPE, typename ALG_OUTPUT_DATA_TYPE, int HLS_INPUT_DATA_BITWIDTH, int HLS_OUTPUT_DATA_BITWIDTH>
class HlsTop {
public:
    HlsTop() {};
    ~HlsTop() {};
    // HlsTop<ALG_INPUT_DATA_TYPE, ALG_OUTPUT_DATA_TYPE, HLS_INPUT_DATA_BITWIDTH, HLS_OUTPUT_DATA_BITWIDTH>() {
    //     MAIN_INFO_1("HlsTop constructor called");
    // }
    // ~HlsTop<ALG_INPUT_DATA_TYPE, ALG_OUTPUT_DATA_TYPE, HLS_INPUT_DATA_BITWIDTH, HLS_OUTPUT_DATA_BITWIDTH>() {
    //     MAIN_INFO_1("HlsTop destructor called");
    // }
    
    // section object
    HlsRegisterSection hls_register_section;
    HlsImageSection hls_image_section;
    HlsOutputSection hls_output_section;

    // data object
    vector<ALG_INPUT_DATA_TYPE> hls_input_image;
    vector<ALG_OUTPUT_DATA_TYPE> hls_output_image;
    
    // ip object
    HlsCrop<HLS_INPUT_DATA_BITWIDTH, HLS_OUTPUT_DATA_BITWIDTH> hls_crop;
    // HlsDpc<HLS_INPUT_DATA_BITWIDTH, HLS_OUTPUT_DATA_BITWIDTH> hls_dpc;


    // section operation
    void loadRegisterSection(const RegisterSection& register_section) {
        MAIN_INFO_1("Register Section loading...");
        hls_register_section.reg_image_width = ap_uint<16>(register_section.reg_map["reg_image_width"]);
        hls_register_section.reg_image_height = ap_uint<16>(register_section.reg_map["reg_image_height"]);
        hls_register_section.reg_crop_enable = (register_section.reg_map["reg_crop_enable"] > 0) ? ap_uint<1>(1) : ap_uint<1>(0);
        hls_register_section.reg_crop_start_x = ap_uint<16>(static_cast<unsigned short>(register_section.reg_map["reg_crop_start_x"]));
        hls_register_section.reg_crop_start_y = ap_uint<16>(static_cast<unsigned short>(register_section.reg_map["reg_crop_start_y"]));
        hls_register_section.reg_crop_end_x = ap_uint<16>(static_cast<unsigned short>(register_section.reg_map["reg_crop_end_x"]));
        hls_register_section.reg_crop_end_y = ap_uint<16>(static_cast<unsigned short>(register_section.reg_map["reg_crop_end_y"]));
        hls_register_section.reg_dpc_enable = (register_section.reg_map["reg_dpc_enable"] > 0) ? ap_uint<1>(1) : ap_uint<1>(0);
        hls_register_section.reg_dpc_threshold = ap_uint<16>(static_cast<unsigned short>(register_section.reg_map["reg_dpc_threshold"]));
    }

    void loadImageSection(const ImageSection& image_section) {
        MAIN_INFO_1("Image Section loading...");
        hls_image_section.image_path = image_section.image_path;
        hls_image_section.random_image_path = image_section.random_image_path;
        hls_image_section.generate_random_image = image_section.generate_random_image;
    }

    void loadOutputSection(const OutputSection& output_section) {
        MAIN_INFO_1("Output Section loading...");
        hls_output_section.alg_crop_output_path = output_section.alg_crop_output_path;
        hls_output_section.alg_dpc_output_path = output_section.alg_dpc_output_path;
        hls_output_section.hls_crop_output_path = output_section.hls_crop_output_path;
        hls_output_section.hls_dpc_output_path = output_section.hls_dpc_output_path;
    }

    void loadSection(RegisterSection& register_section, const ImageSection& image_section, const OutputSection& output_section) {
        loadRegisterSection(register_section);
        loadImageSection(image_section);
        loadOutputSection(output_section);
    }

    void loadImage() {
        MAIN_INFO_1("Image loading...");
        if (hls_image_section.generate_random_image) {
            hls_input_image = vector_read_from_file<ALG_INPUT_DATA_TYPE>(hls_image_section.random_image_path);
        } else {
            hls_input_image = vector_read_from_file<ALG_INPUT_DATA_TYPE>(hls_image_section.image_path);
        }
    }


    void printRegisterSection() {
        MAIN_INFO_1("Register Section printing...");
        cout << "reg_image_width: " << (uint16_t)hls_register_section.reg_image_width << endl;
        cout << "reg_image_height: " << (uint16_t)hls_register_section.reg_image_height << endl;
        cout << "reg_crop_enable: " << (bool)hls_register_section.reg_crop_enable << endl;
        cout << "reg_crop_start_x: " << (uint16_t)hls_register_section.reg_crop_start_x << endl;
        cout << "reg_crop_start_y: " << (uint16_t)hls_register_section.reg_crop_start_y << endl;
        cout << "reg_crop_end_x: " << (uint16_t)hls_register_section.reg_crop_end_x << endl;
        cout << "reg_crop_end_y: " << (uint16_t)hls_register_section.reg_crop_end_y << endl;
        cout << "reg_dpc_enable: " << (bool)hls_register_section.reg_dpc_enable << endl;
        cout << "reg_dpc_threshold: " << (uint16_t)hls_register_section.reg_dpc_threshold << endl;
    }

    void printImageSection() {
        MAIN_INFO_1("Image Section printing...");
        cout << "image_path: " << hls_image_section.image_path << endl;
        cout << "random_image_path: " << hls_image_section.random_image_path << endl;
        cout << "generate_random_image: " << hls_image_section.generate_random_image << endl;
    }

    void printOutputSection() {
        MAIN_INFO_1("Output Section printing...");
        cout << "alg_crop_output_path: " << hls_output_section.alg_crop_output_path << endl;
        cout << "alg_dpc_output_path: " << hls_output_section.alg_dpc_output_path << endl;
        cout << "hls_crop_output_path: " << hls_output_section.hls_crop_output_path << endl;
        cout << "hls_dpc_output_path: " << hls_output_section.hls_dpc_output_path << endl;
    }

    void printSection() {
        printRegisterSection();
        printImageSection();
        printOutputSection();
    }

    // template <typename T, typename U>
    // hls::stream<U>& vector_to_stream(const vector<T>& data) {
    //     hls::stream<U> stream;
    //     for (size_t i = 0; i < data.size(); ++i) {
    //         U data_pkt;
    //         if (data[i] >= 256) {
    //             std::cerr << "Warning: Value " << data[i] << " exceeds 8-bit range, truncating" << std::endl;
    //             data_pkt.data = 255; // 8-bit max
    //         } else {
    //             data_pkt.data = static_cast<U>(data[i]);
    //         }
    //         data_pkt.last = (i == data.size() - 1) ? 1 : 0;
    //         stream.write(data_pkt);
    //     }
    //     return stream;
    // }

    // template <typename T, typename U>
    // vector<T> stream_to_vector(hls::stream<U>& stream) {
    //     vector<T> data;
    //     while (!stream.empty()) {
    //         auto data_pkt = stream.read();
    //         data.push_back(static_cast<T>(data_pkt.data));
    //     }
    //     return data;
    // }

    void run(RegisterSection& register_section, const ImageSection& image_section, const OutputSection& output_section) {
        // hls initialize
        MAIN_INFO_1("hls initialize...");
        loadSection(register_section, image_section, output_section);
        printSection();
        loadImage();

        // hls run
        MAIN_INFO_1("hls run...");      
        // data object
        hls::stream<ap_axiu<HLS_INPUT_DATA_BITWIDTH, 0, 0, 0>> hls_input_stream;
        hls::stream<ap_axiu<HLS_OUTPUT_DATA_BITWIDTH, 0, 0, 0>> hls_output_stream;

        MAIN_INFO_1("hls crop run simulation (skipping actual HLS code)...");

        vector_to_stream(hls_input_image, hls_input_stream);
        hls_crop.run(hls_input_stream, hls_output_stream, hls_register_section);
        stream_to_vector(hls_output_stream, hls_output_image);

        // Write output to file
        int crop_image_width = hls_register_section.reg_crop_end_x-hls_register_section.reg_crop_start_x+1;
        int crop_image_height = hls_register_section.reg_crop_end_y-hls_register_section.reg_crop_start_y+1;
        MAIN_INFO_1("hls crop output image width: " + std::to_string(crop_image_width));
        MAIN_INFO_1("hls crop output image height: " + std::to_string(crop_image_height));
        vector_write_to_file(hls_output_section.hls_crop_output_path, hls_output_image, crop_image_width, crop_image_height);
        MAIN_INFO_1("hls crop output data save to: " + hls_output_section.hls_crop_output_path);
        MAIN_INFO_1("hls run completed");
    }


};


#endif
