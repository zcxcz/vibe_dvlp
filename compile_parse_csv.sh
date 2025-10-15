#!/bin/bash

echo "开始编译 parse_csv_function.cpp..."

# 编译参数
CXX=g++
CXXFLAGS="-std=c++11 -Wall -Wextra -O2"
INCLUDES="-I./src -I./src/hls_lib"

# 源文件
SRCS="src/parse_csv_function.cpp src/print_function.cpp src/vector_function.cpp"

# 输出文件
OUTPUT="parse_csv"

# 编译命令
$CXX $CXXFLAGS $INCLUDES $SRCS -o $OUTPUT

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "编译成功！生成可执行文件: $OUTPUT"
    echo "运行程序..."
    ./$OUTPUT
else
    echo "编译失败！"
    exit 1
fi