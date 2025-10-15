#!/bin/bash

echo "开始编译 output_compare_main.cpp..."

# 编译参数
CXX=g++
CXXFLAGS="-std=c++11 -Wall -Wextra -O2"
INCLUDES="-I./src -I./src/hls_lib"

# 源文件
SRCS="src/output_compare_main.cpp src/print_function.cpp src/vector_function.cpp"

# 输出文件
OUTPUT="output_compare_main"

# 编译命令
$CXX $CXXFLAGS $INCLUDES $SRCS -o $OUTPUT

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "编译成功！生成可执行文件: $OUTPUT"
else
    echo "编译失败！"
    exit 1
fi