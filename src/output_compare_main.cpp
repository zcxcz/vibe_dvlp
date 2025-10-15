// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>

// tool
#include "json.hpp"
#include "print_function.h"
#include "vector_function.h"
#include "parse_json_function.h"

// using
using json = nlohmann::json;
using namespace std;

// def
#define ALG_INPUT_DATA_TYPE uint16_t
#define ALG_OUTPUT_DATA_TYPE uint16_t
#define HLS_INPUT_DATA_BITWIDTH     8
#define HLS_OUTPUT_DATA_BITWIDTH    8


int main(const int argc, const char *argv[]) {
    string config_path = "/home/sheldon/hls_project/vibe_crop/src/vibe.json";
    ifstream f(config_path);
    if (!f.is_open()) {
        MAIN_ERROR_1("Cannot open vibe.json configuration file");
        return -1;
    }
    MAIN_INFO_1("vibe.json configuration file path: " + config_path);
    
    json data = json::parse(f);
    f.close();
    MAIN_INFO_1("vibe.json configuration file parse follow...");
    
    // 检查 output_info 是否存在
    if (!data.contains("output_info")) {
        MAIN_ERROR_1("output_info not found in vibe.json");
        return -1;
    }
    
    json output_info = data["output_info"];
    MAIN_INFO_1("output_info content: " + output_info.dump(2));
    
    // 比较结果统计
    int total_comparisons = 0;
    int successful_comparisons = 0;
    
    // 遍历 output_info 中的所有键值对
    for (auto& [key, value] : output_info.items()) {
        // 只处理 alg_ 开头的文件路径
        if (key.substr(0, 4) == "alg_") {
            total_comparisons++;
            
            // 生成对应的 hls_ 键名
            string hls_key = "hls" + key.substr(3); // 替换 alg_ 为 hls_
            
            // 检查对应的 hls_ 键是否存在
            if (!output_info.contains(hls_key)) {
                MAIN_ERROR_1("Corresponding HLS key not found for: " + key);
                continue;
            }
            
            string alg_file_path = value.get<string>();
            string hls_file_path = output_info[hls_key].get<string>();
            
            MAIN_INFO_1("Comparing files:");
            MAIN_INFO_1("  Algorithm file: " + alg_file_path);
            MAIN_INFO_1("  HLS file: " + hls_file_path);
            
            // 读取两个文件的内容
            vector<ALG_OUTPUT_DATA_TYPE> alg_data = vector_read_from_file<ALG_OUTPUT_DATA_TYPE>(alg_file_path);
            if (alg_data.empty()) {
                MAIN_ERROR_1("Cannot load algorithm output file: " + alg_file_path);
                continue;
            }
            
            vector<ALG_OUTPUT_DATA_TYPE> hls_data = vector_read_from_file<ALG_OUTPUT_DATA_TYPE>(hls_file_path);
            if (hls_data.empty()) {
                MAIN_ERROR_1("Cannot load HLS output file: " + hls_file_path);
                continue;
            }
            
            MAIN_INFO_1("  Algorithm data size: " + to_string(alg_data.size()));
            MAIN_INFO_1("  HLS data size: " + to_string(hls_data.size()));
            
            // 调用 vector_compare 进行比较
            bool comparison_result = vector_compare(alg_data, hls_data);
            
            if (comparison_result) {
                MAIN_INFO_1("  Comparison result: SUCCESS - Files are identical");
                successful_comparisons++;
            } else {
                MAIN_INFO_1("  Comparison result: FAILED - Files are different");
            }
        }
    }
    
    // 输出比较结果摘要
    MAIN_INFO_1("\nComparison Summary:");
    MAIN_INFO_1("Total comparisons: " + to_string(total_comparisons));
    MAIN_INFO_1("Successful comparisons: " + to_string(successful_comparisons));
    MAIN_INFO_1("Failed comparisons: " + to_string(total_comparisons - successful_comparisons));
    
    if (total_comparisons > 0 && successful_comparisons == total_comparisons) {
        MAIN_INFO_1("All comparisons passed successfully!");
    } else if (total_comparisons > 0) {
        MAIN_INFO_1("Some comparisons failed. Please check the details above.");
    } else {
        MAIN_INFO_1("No comparison files found in output_info.");
    }
    
    return 0;
}