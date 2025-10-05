#ifndef ALG_TOP_H
#define ALG_TOP_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <string>

// tool
#include "image_info.h"
#include "print_function.h"
#include "json.hpp"

// ip
#include "alg_crop.h"
#include "alg_dpc.h"

using json = nlohmann::json;
using namespace std;

// def
#define ALG_TOP_SECTION "AlgTop"

// Forward declarations
struct AlgRegisterSection {
    // register info
    int width;
    int height;
    int crop_start_x;
    int crop_start_y;
    int crop_end_x;
    int crop_end_y;
    bool crop_enable;
    bool dpc_enable;
    int dpc_threshold;
};

struct AlgImageSection {
    // image info
    string random_image_path;
    string input_file;
    int generate_random_image;
};

struct AlgOutputSection {
    // image info
    string crop_output_file;
    string dpc_output_file;
};

class AlgTop {
public:
    AlgTop();
    ~AlgTop();
    
    AlgRegisterSection alg_register_section;
    AlgImageSection alg_image_section;
    AlgOutputSection alg_output_section;
    
    void run(const RegisterSection& config);
    void loadRegisterSection(const RegisterSection& register_section);
    void loadImageSection(const ImageSection& image_section);
    void loadOutputSection(const OutputSection& output_section);
    void loadSection(const RegisterSection& register_section, const ImageSection& image_section, const OutputSection& output_section);
    void printRegisterSection();
    void printImageSection();
    void printOutputSection();
    void printSection();
};

#endif // ALG_TOP_H