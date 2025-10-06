#!/bin/bash

echo "开始编译 hls_main.cpp..."

# 编译参数
CXX=g++
CXXFLAGS="-std=c++11 -Wall -Wextra -O2"
INCLUDES="-I./src -I./src/hls_lib"

# 源文件
SRCS="src/hls_main.cpp src/hls_top.cpp src/print_function.cpp src/vector_function.cpp"

# 输出文件
OUTPUT="hls_main"

# 编译命令
$CXX $CXXFLAGS $INCLUDES $SRCS -o $OUTPUT

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "编译成功！生成可执行文件: $OUTPUT"
else
    echo "编译失败！"
    exit 1
fi