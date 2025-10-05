// std
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <string>

// tool
#include "parse_json_function.h"
#include "print_function.h"
#include "alg_top.h"

// ip
#include "alg_crop.h"
#include "alg_dpc.h"

// using
using json = nlohmann::json;
using namespace std;

// 构造函数和析构函数的实现
template <typename ALG_INPUT_DATA_TYPE, typename ALG_OUTPUT_DATA_TYPE>
AlgTop<ALG_INPUT_DATA_TYPE, ALG_OUTPUT_DATA_TYPE>::AlgTop() {
    // 构造函数实现
}

template <typename ALG_INPUT_DATA_TYPE, typename ALG_OUTPUT_DATA_TYPE>
AlgTop<ALG_INPUT_DATA_TYPE, ALG_OUTPUT_DATA_TYPE>::~AlgTop() {
    // 析构函数实现
}

// 显式模板实例化
template class AlgTop<unsigned short, unsigned short>;
