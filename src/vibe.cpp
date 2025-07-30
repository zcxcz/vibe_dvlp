#include "json.hpp"
#include <iostream>
#include <fstream>
#include <vector>

using json = nlohmann::json;
using namespace std;

struct ImageInfo {
    string image_path;
    string image_format;
    int image_data_bitwidth;
    
    // Print image information
    void print_values() const {
        cout << "=== Image Information ===" << endl;
        cout << "Image Path: " << image_path << endl;
        cout << "Image Format: " << image_format << endl;
        cout << "Image Data Bitwidth: " << image_data_bitwidth << endl;
        cout << "========================" << endl;
    }
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ImageInfo, image_path, image_format, image_data_bitwidth)
};

struct RegInfo {
    int reg_bit_width;
    vector<int> reg_initial_value;
    int reg_value_min;
    int reg_value_max;
    
    // Determine if it's a single value - check by vector size
    bool is_single_value() const {
        return reg_initial_value.size() == 1;
    }
    
    // Overload [] operator to support seamless access
    int operator[](size_t index) const {
        if (reg_initial_value.empty()) {
            return 0;
        }
        return index < reg_initial_value.size() ? reg_initial_value[index] : 0;
    }
    
    // Get the number of values
    size_t size() const {
        return reg_initial_value.empty() ? 1 : reg_initial_value.size();
    }
    
    // Unified printing function, automatically handle single value or array
    void print_values(const string& name = "Values") const {
        cout << name << ": ";
        if (reg_initial_value.empty()) {
            cout << "0";
        } else if (is_single_value()) {
            cout << reg_initial_value[0];
        } else {
            for (size_t i = 0; i < reg_initial_value.size(); ++i) {
                cout << reg_initial_value[i];
                if (i < reg_initial_value.size() - 1) {
                    cout << " ";
                }
            }
        }
        cout << endl;
    }
    
    // Helper functions for JSON serialization
    friend void to_json(json& j, const RegInfo& r) {
        // If there's only one value, serialize as a single integer; otherwise serialize as an array
        if (r.reg_initial_value.size() == 1) {
            j = json{{"reg_bit_width", r.reg_bit_width}, {"reg_initial_value", r.reg_initial_value[0]}, {"reg_value_min", r.reg_value_min}, {"reg_value_max", r.reg_value_max}};
        } else {
            j = json{{"reg_bit_width", r.reg_bit_width}, {"reg_initial_value", r.reg_initial_value}, {"reg_value_min", r.reg_value_min}, {"reg_value_max", r.reg_value_max}};
        }
    }
    
    friend void from_json(const json& j, RegInfo& r) {
        j.at("reg_bit_width").get_to(r.reg_bit_width);
        j.at("reg_value_min").get_to(r.reg_value_min);
        j.at("reg_value_max").get_to(r.reg_value_max);
        
        // Handle reg_initial_value which could be a single integer or an array of integers
        if (j.at("reg_initial_value").is_number()) {
            int single_value;
            j.at("reg_initial_value").get_to(single_value);
            r.reg_initial_value = {single_value};
        } else {
            j.at("reg_initial_value").get_to(r.reg_initial_value);
        }
    }
};

struct RegisterInfo {
    RegInfo reg_image_width;
    RegInfo reg_image_height;
    RegInfo reg_filter_coeff;
    RegInfo reg_crop_width;
    RegInfo reg_crop_height;
    RegInfo reg_crop_enable;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RegisterInfo, reg_image_width, reg_image_height, reg_filter_coeff, reg_crop_width, reg_crop_height, reg_crop_enable)
    
    // Print all register information
    void print_values() const {
        cout << "=== Register Information ===" << endl;
        
        // Use helper function to print member information
        print_reg_info("Reg Image Width", reg_image_width);
        print_reg_info("Reg Image Height", reg_image_height);
        print_reg_info("Reg Filter Coeff", reg_filter_coeff);
        print_reg_info("Reg Crop Width", reg_crop_width);
        print_reg_info("Reg Crop Height", reg_crop_height);
        print_reg_info("Reg Crop Enable", reg_crop_enable);
        
        cout << "==========================" << endl;
    }
    
private:
    // Helper function to print information of a single RegInfo member
    void print_reg_info(const string& name, const RegInfo& reg) const {
        cout << name << ":" << endl;
        cout << "  Bit Width: " << reg.reg_bit_width << endl;
        cout << "  Min Value: " << reg.reg_value_min << endl;
        cout << "  Max Value: " << reg.reg_value_max << endl;
        reg.print_values("  Initial Value");
    }
};

int main(const int argc, const char *argv[]) {
    try {
        // Read JSON file
        ifstream f("./data/vibe.json");
        json data = json::parse(f);
        
        // Directly convert JSON data to struct
        ImageInfo img_info = data["image_info"].get<ImageInfo>();
        RegisterInfo reg_info = data["register_info"].get<RegisterInfo>();
        
        // Get image parameters (using [] operator directly)
        int width = reg_info.reg_image_width[0];
        int height = reg_info.reg_image_height[0];

        string image_path = img_info.image_path;
        string image_format = img_info.image_format;
        
        // Print image and register` information using unified print_values function
        cout << "=== Image Information ===" << endl;
        img_info.print_values();
        cout << "=== Register Information ===" << endl;
        reg_info.print_values();
        
        // Generate random image data
        cout << "Generating random image data..." << endl;
        string command = string("python ./py/generate_random_image.py") +
            " --width " + to_string(width) + 
            " --height " + to_string(height) + 
            " --format " + image_format +
            " --output " + image_path;
        
        int result = system(command.c_str());
        if (result == 0) {
            cout << "Random image generated successfully: " << image_path << endl;
        } else {
            cerr << "Failed to generate random image" << endl;
        }
        
        // Crop processing
        cout << "Starting crop processing..." << endl;
        int crop_width = reg_info.reg_crop_width[0];
        int crop_height = reg_info.reg_crop_height[0];
        int crop_enable = reg_info.reg_crop_enable[0];
        
        string crop_command = string("python ./py/crop_adapter.py") +
            " ./data/test.img" +
            " ./data/crop_output.img" +
            " " + to_string(width) +
            " " + to_string(height) +
            " " + to_string(crop_width) +
            " " + to_string(crop_height) +
            " " + to_string(crop_enable) +
            " " + to_string(img_info.image_data_bitwidth) +
            " " + img_info.image_format;
        
        int crop_result = system(crop_command.c_str());
        if (crop_result == 0) {
            cout << "Crop processing completed successfully" << endl;
        } else {
            cerr << "Failed to execute crop processing" << endl;
        }
        
    } catch (json::parse_error& e) {
        cerr << "JSON parsing error: " << e.what() << endl;
        return 1;
    } catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
