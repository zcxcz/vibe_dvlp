#!/bin/bash

# 构建脚本，用于编译和运行算法和HLS Top模块

# 设置变量
ALG_TOP_SRC="src/alg_top.cpp src/alg_crop.cpp src/alg_dpc.cpp src/alg_info.cpp"
HLS_TOP_SRC="src/hls_top.cpp src/hls_crop.cpp src/hls_dpc.cpp src/alg_info.cpp"
ALG_TOP_EXE="alg_top"
HLS_TOP_EXE="hls_top"
CXX="g++"
CXXFLAGS="-std=c++11 -O2 -I./src"

# 构建算法Top模块
build_alg_top() {
    echo "Building algorithm top module..."
    $CXX $CXXFLAGS $ALG_TOP_SRC -o $ALG_TOP_EXE
    if [ $? -ne 0 ]; then
        echo "Failed to build algorithm top module"
        return 1
    fi
    echo "Algorithm top module built successfully"
    return 0
}

# 构建HLS Top模块
build_hls_top() {
    echo "Building HLS top module..."
    # 设置HLS包含路径，优先使用本地hls_lib
    HLS_INC="-I./src/hls_lib"
    echo "Using local HLS include path: $HLS_INC"
    
    # 如果定义了XILINX_HLS环境变量，也包含其include目录
    if [ ! -z "$XILINX_HLS" ]; then
        HLS_INC="$HLS_INC -I$XILINX_HLS/include"
        echo "Also using Xilinx HLS include path: $XILINX_HLS/include"
    fi
    
    $CXX $CXXFLAGS $HLS_INC $HLS_TOP_SRC -o $HLS_TOP_EXE
    if [ $? -ne 0 ]; then
        echo "Failed to build HLS top module"
        return 1
    fi
    echo "HLS top module built successfully"
    return 0
}

# 运行算法Top模块
run_alg_top() {
    if [ ! -f $ALG_TOP_EXE ]; then
        echo "Algorithm top module executable not found"
        return 1
    fi
    echo "Running algorithm top module..."
    ./$ALG_TOP_EXE data/vibe.json
    return $?
}

# 运行HLS Top模块
run_hls_top() {
    if [ ! -f $HLS_TOP_EXE ]; then
        echo "HLS top module executable not found"
        return 1
    fi
    echo "Running HLS top module..."
    ./$HLS_TOP_EXE data/vibe.json
    return $?
}

# 帮助信息
show_help() {
    echo "Usage: $0 [options]"
    echo "Options:" 
    echo "  build_alg      Build algorithm top module"
    echo "  build_hls      Build HLS top module"
    echo "  build_all      Build both algorithm and HLS top modules"
    echo "  run_alg        Run algorithm top module"
    echo "  run_hls        Run HLS top module"
    echo "  run_all        Run both algorithm and HLS top modules"
    echo "  clean          Clean build files"
    echo "  help           Show this help message"
    return 0
}

# 清理构建文件
clean() {
    echo "Cleaning build files..."
    rm -f $ALG_TOP_EXE $HLS_TOP_EXE
    echo "Done"
    return 0
}

# 主函数
main() {
    if [ $# -eq 0 ]; then
        show_help
        return 0
    fi
    
    case $1 in
        build_alg)
            build_alg_top
            ;;
        build_hls)
            build_hls_top
            ;;
        build_all)
            build_alg_top && build_hls_top
            ;;
        run_alg)
            run_alg_top
            ;;
        run_hls)
            run_hls_top
            ;;
        run_all)
            run_alg_top && run_hls_top
            ;;
        clean)
            clean
            ;;
        help)
            show_help
            ;;
        *)
            echo "Invalid option: $1"
            show_help
            return 1
            ;;
    esac
    
    return 0
}

# 执行主函数
main "$@"

exit $?