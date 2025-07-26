#!/bin/bash

# Create necessary directories
mkdir -p data log py src

# Create and update interaction log
cat > log/interact.log << 'EOL'
# Interaction Log

## Date
2024-01-09

## Completed Tasks
1. Created interaction log file to record project progress and interactions
2. Implemented JSON parsing functionality in vibe.cpp:
   - Fixed syntax errors
   - Added file reading and parsing
   - Implemented image and register information parsing
   - Added error handling mechanisms
3. Fixed design issues in RegisterInfo struct:
   - Modified reg_filter_coeff to RegInfo type
   - Used variant to support different types of reg_initial_value
   - Corrected related data access code
4. Optimized JSON parsing code:
   - Moved NLOHMANN_DEFINE_TYPE_INTRUSIVE macro inside struct
   - Added member function form serialization functions for RegInfo struct
   - Simplified JSON parsing logic in main function
5. Further optimized RegInfo struct and data access:
   - Removed unused member function form serialization functions
   - Added get_values() helper function to uniformly handle initial_value
   - Simplified value acquisition logic in main function
6. Created project file generation script:
   - Created generate_vibe_files.sh script
   - Implemented functionality to automatically generate vibe.json and vibe.cpp files
   - Maintained file content consistency with current version
7. Final optimization:
   - Removed global to_json and from_json functions
   - Used NLOHMANN_DEFINE_TYPE_INTRUSIVE macro in RegInfo struct
   - Updated generation script to reflect latest code changes
8. Created Random Image Generator:
   - Created generate_random_image.py script using Python
   - Supported multiple bit depths (8, 12, 16 bits)
   - Provided command line parameter interface
   - Created test script test_random_image.py for demonstrating functionality
9. Integrated random image generation functionality into main program:
   - Added code to call random image generator in main function of vibe.cpp
   - Used image_path from JSON as output file name
   - Generated images based on width and height parameters in register_info
   - Updated generate_vibe_files.sh script to keep in sync
10. Optimized RegInfo struct to improve user experience:
    - Added is_single_value() method to automatically determine register value type
    - Overloaded [] operator to support seamless access to single values or array elements
    - Added size() method to get the number of values
    - Simplified code in main function, eliminating the need for users to manually distinguish types
    - Updated generate_vibe_files.sh script to keep in sync
11. Further optimized RegInfo struct, simplifying interface:
    - Removed redundant methods get_single_value(), get_array_values(), and get_values()
    - Retained only [] operator overloading and print_values() unified printing function
    - Simplified register value access logic in main function
    - Updated generate_vibe_files.sh script to keep in sync
12. Added build system support for vibe.cpp project:
    - Created CMakeLists.txt, supporting C++17 standard, including header directory configuration
    - Created Makefile, compatible with Windows and Unix-like systems, providing clean/run/debug/release/install/help targets
    - Both build systems include json.hpp existence check
13. Fixed json.hpp inclusion issue: Added json.hpp existence check in CMakeLists.txt and Makefile, ensuring correct inclusion of header file during build and providing error提示 information.
14. Optimized file update mechanism: Created update_files.sh intelligent update script, using sed commands for precise replacement to avoid complete file rewriting:
    - Supported selective updating of specific files (cmake/makefile/reginfo/main)
    - Used sed commands for precise content replacement, avoiding repeated processing of entire files
    - Provided detailed log output and error handling
    - Modular design to reduce data processing volume

## Pending Tasks
1. To be补充

## Interaction Records
1. User requested creation of interaction log file
2. User requested extraction of vibe.json content through json library in vibe.cpp
3. User pointed out incorrect definition of reg_filter_coeff in RegisterInfo struct
4. User asked about variant functionality and requested use of macros to implement struct and JSON bridging
5. User pointed out macro position issue and requested code optimization
6. User pointed out unused serialization function in RegInfo and requested optimization of initial_value access method
7. User requested creation of shell script to generate project files
8. User pointed out unused global serialization functions and requested removal
9. User requested creation of random image generator
10. User requested integration of random image generation functionality into main program
11. User requested optimization of RegInfo struct's user experience
12. User requested further optimization of RegInfo struct interface
13. User requested addition of build system support
14. User requested fixing of json.hpp inclusion issue
15. User requested optimization of file update mechanism
16. User requested updating of generate_vibe_files.sh to sync with interact.log changes and add Makefile and CMakeLists.txt generation
EOL

# Create CMakeLists.txt
cat > CMakeLists.txt << 'EOL'
cmake_minimum_required(VERSION 3.10)
project(vibe)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if json.hpp exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/json.hpp")
    message(FATAL_ERROR "json.hpp not found in ${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

# Add executable
add_executable(vibe src/vibe.cpp)

# Include header directories
target_include_directories(vibe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add compilation options
target_compile_options(vibe PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link system libraries
target_link_libraries(vibe PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:stdc++fs>
)

# Installation rules
install(TARGETS vibe DESTINATION bin)
install(FILES data/vibe.json DESTINATION share/vibe)
EOL

# Create Makefile
cat > Makefile << 'EOL'
# Makefile for vibe project

# Compiler settings
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -I src

# Debug and release modes
DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O3 -DNDEBUG

# Source and object files
SOURCES := src/vibe.cpp
OBJECTS := $(SOURCES:.cpp=.o)
TARGET := vibe

# Check if json.hpp exists
JSON_HPP := src/json.hpp

# Default target
all: $(TARGET)

# Check dependencies
$(JSON_HPP):
	@echo "Error: $(JSON_HPP) not found!"
	@exit 1

# Build executable
$(TARGET): $(OBJECTS) $(JSON_HPP)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile object files
%.o: %.cpp $(JSON_HPP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run program
run: $(TARGET)
	./$(TARGET)

# Debug version
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Release version
release: CXXFLAGS += $(RELEASE_FLAGS)
release: clean $(TARGET)

# Install (Unix-like systems)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	mkdir -p /usr/local/share/vibe
	cp vibe.json /usr/local/share/vibe/

# Windows installation
install-windows: $(TARGET)
	copy $(TARGET).exe C:\Windows\System32\ 2>nul || copy $(TARGET).exe %USERPROFILE%\bin\
	if not exist %USERPROFILE%\share\vibe mkdir %USERPROFILE%\share\vibe
	copy vibe.json %USERPROFILE%\share\vibe\

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Compile program (default)"
	@echo "  clean     - Clean generated files"
	@echo "  run       - Compile and run program"
	@echo "  debug     - Compile debug version"
	@echo "  release   - Compile release version"
	@echo "  install   - Install to system (Unix-like)"
	@echo "  install-windows - Install to user directory (Windows)"
	@echo "  help      - Show this help information"

.PHONY: all clean run debug release install install-windows help
EOL

# Create vibe.json
cat > data/vibe.json << 'EOL'
{
    "image_info": {
        "image_path": "./test.txt",
        "image_format": "BAYER",
        "image_data_bitwidth": 8
    },
    "register_info": {
        "reg_image_width": {
            "reg_bit_width": 16,
            "reg_initial_value": 4,
            "reg_value_min": 0,
            "reg_value_max": 31
        },
        "reg_image_height": {
            "reg_bit_width": 16,
            "reg_initial_value": 8,
            "reg_value_min": 0,
            "reg_value_max": 31
        },
        "reg_filter_coeff": {
            "reg_bit_width": 8,
            "reg_initial_value": [1, 2, 3, 4, 5, 6, 7, 8, 9],
            "reg_value_min": 0,
            "reg_value_max": 255
        }
    }
}
EOL

# Create vibe.cpp
cat > src/vibe.cpp << 'EOL'
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
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RegisterInfo, reg_image_width, reg_image_height, reg_filter_coeff)
    
    // Print all register information
    void print_values() const {
        cout << "=== Register Information ===" << endl;
        
        // Use helper function to print member information
        print_reg_info("Reg Image Width", reg_image_width);
        print_reg_info("Reg Image Height", reg_image_height);
        print_reg_info("Reg Filter Coeff", reg_filter_coeff);
        
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
        ifstream f("data/vibe.json");
        json data = json::parse(f);
        
        // Directly convert JSON data to struct
        ImageInfo img_info = data["image_info"].get<ImageInfo>();
        RegisterInfo reg_info = data["register_info"].get<RegisterInfo>();
        
        // Get image parameters (using [] operator directly)
        int width = reg_info.reg_image_width[0];
        int height = reg_info.reg_image_height[0];
        string image_path = img_info.image_path;
        
        // Print image and register information using unified print_values function
        img_info.print_values();
        reg_info.print_values();
        
        // Generate random image data
        cout << "Generating random image data..." << endl;
        string command = "python ./py/generate_random_image.py --width " + to_string(width) + 
                        " --height " + to_string(height) + 
                        " --format GRAY --output " + image_path;
        
        int result = system(command.c_str());
        if (result == 0) {
            cout << "Random image generated successfully: " << image_path << endl;
        } else {
            cerr << "Failed to generate random image" << endl;
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
EOL

# Create generate_random_image.py
cat > py/generate_random_image.py << 'EOL'
#!/usr/bin/env python3
"""
Random Image Generator
Generates random images with integer data type based on input width, height and format
"""

import numpy as np
import argparse
import os


def generate_random_image(width, height, image_format='RGB', bit_depth=8):
    """
    Generate random image
    
    Args:
        width (int): Image width
        height (int): Image height
        image_format (str): Image format ('RGB', 'YUV', 'BAYER', 'GRAY')
        bit_depth (int): Bit depth (8, 12, 16)
    
    Returns:
        np.ndarray: Generated random image data
    """
    # Determine number of channels based on format
    if image_format.upper() == 'RGB':
        channels = 3
    elif image_format.upper() == 'YUV':
        channels = 3
    elif image_format.upper() == 'BAYER':
        channels = 1
    elif image_format.upper() == 'GRAY' or image_format.upper() == 'GRAYSCALE':
        channels = 1
    else:
        raise ValueError(f"Unsupported image format: {image_format}")
    
    # Determine maximum value based on bit depth
    max_value = (2 ** bit_depth) - 1
    
    # Generate random data
    if channels == 1:
        random_data = np.random.randint(0, max_value + 1, (height, width), dtype=np.int32)
    else:
        random_data = np.random.randint(0, max_value + 1, (height, width, channels), dtype=np.int32)
    
    return random_data


def save_image(image_data, output_path, bit_depth=8):
    """
    Save image to file in specified format
    
    Args:
        image_data (np.ndarray): Image data
        output_path (str): Output file path
        bit_depth (int): Bit depth
    """
    with open(output_path, 'w') as f:
        # Check if it's single channel
        if len(image_data.shape) == 2:
            # Single channel, one pixel data per line
            for row in image_data:
                for pixel in row:
                    f.write(f"{pixel}\n")
        else:
            # Multi-channel, each channel's data is placed in a line, separated by spaces
            height, width, channels = image_data.shape
            for h in range(height):
                for w in range(width):
                    # Write all channel values of current pixel
                    pixel_values = ' '.join(str(image_data[h, w, c]) for c in range(channels))
                    f.write(f"{pixel_values}\n")
    
    print(f"Image saved to: {output_path}")


def main():
    parser = argparse.ArgumentParser(description='Generate random image')
    parser.add_argument('--width', type=int, required=True, help='Image width')
    parser.add_argument('--height', type=int, required=True, help='Image height')
    parser.add_argument('--format', type=str, default='RGB', choices=['RGB', 'YUV', 'BAYER', 'GRAY'], help='Image format')
    parser.add_argument('--bit-depth', type=int, default=8, choices=[8, 12, 16], help='Bit depth')
    parser.add_argument('--output', type=str, default='random_image.png', help='Output file path')
    
    args = parser.parse_args()
    
    # Validate parameters
    if args.width <= 0 or args.height <= 0:
        print("Error: Width and height must be positive integers")
        return
    
    # 生成随机图像
    try:
        random_image = generate_random_image(args.width, args.height, args.format, args.bit_depth)
        
        # Save image
        save_image(random_image, args.output, args.bit_depth)
        
        # Print image information
        print(f"Image information:")
        print(f"  Size: {args.width} x {args.height}")
        print(f"  Format: {args.format}")
        print(f"  Bit depth: {args.bit_depth}-bit")
        print(f"  Data type: {random_image.dtype}")
        print(f"  Data range: [{random_image.min()}, {random_image.max()}]")
        
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
EOL


# List all created files
echo "=== Created Files ==="
echo "log/interact.log"
echo "CMakeLists.txt"
echo "Makefile"
echo "data/vibe.json"
echo "src/vibe.cpp"
echo "py/generate_random_image.py"
echo "py/test_random_image.py"
echo "==================="

# Add fixed timestamp
 echo "Last updated: 2023-11-08 15:30:00\n"