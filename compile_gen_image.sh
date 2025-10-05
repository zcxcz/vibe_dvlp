#!/bin/bash

# 编译 gen_image_main.cpp 的脚本

echo "开始编译 gen_image_main.cpp..."

# 设置编译参数
CXX=g++
CXXFLAGS="-std=c++11 -Wall -Wextra"

# 源文件列表
SRCS="src/gen_image_main.cpp src/print_function.cpp src/vector_function.cpp"

# 输出可执行文件
OUTPUT="gen_image_main"

# 执行编译
$CXX $CXXFLAGS $SRCS -o $OUTPUT

if [ $? -eq 0 ]; then
    echo "编译成功！生成可执行文件: $OUTPUT"
else
    echo "编译失败！"
    exit 1
fi