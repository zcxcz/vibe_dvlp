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
    echo "  compare        Compare algorithm and HLS output files"
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

# 智能对比算法和HLS输出文件
compare_outputs() {
    echo "Comparing algorithm and HLS output files..."
    
    # 查找data目录下所有符合alg*output.txt模式的文件
    ALG_FILES=($(find data -name "alg*output.txt" 2>/dev/null))
    
    # 检查是否找到算法输出文件
    if [ ${#ALG_FILES[@]} -eq 0 ]; then
        echo "Warning: No algorithm output files found (expected pattern: alg*output.txt)"
        echo "Try './build_top.sh run_all' to generate the output files."
        return 1
    fi
    
    # 初始化变量用于统计
    TOTAL_FILES=0
    MATCHED_FILES=0
    ALL_MATCH=true
    
    # 收集所有文件以进行统计
    ALL_FILES=()
    
    # 遍历所有算法输出文件，查找对应的HLS输出文件
    for alg_file in "${ALG_FILES[@]}"; do
        # 提取模式部分（例如从data/alg_crop_output.txt提取crop）
        # 移除目录部分
        alg_file_basename=$(basename "$alg_file")
        # 提取模式
        pattern="${alg_file_basename#alg_}"  # 移除前缀alg_
        pattern="${pattern%_output.txt}"  # 移除后缀_output.txt
        
        # 构建对应的HLS文件名
        hls_file="data/hls_${pattern}_output.txt"
        
        # 添加到总文件列表
        ALL_FILES+=("$alg_file" "$hls_file")
        TOTAL_FILES=$((TOTAL_FILES + 1))
        
        # 检查HLS文件是否存在
        if [ ! -f "$hls_file" ]; then
            echo "Warning: Corresponding HLS file not found for $alg_file: $hls_file"
            ALL_MATCH=false
            continue
        fi
        
        # 对匹配的文件对进行对比
        echo -e "\n=== Comparing $pattern Results ==="
        if diff -q "$alg_file" "$hls_file" > /dev/null; then
            echo "✅ $pattern results match between algorithm and HLS implementation!"
            MATCHED_FILES=$((MATCHED_FILES + 1))
        else
            echo "❌ $pattern results differ between algorithm and HLS implementation."
            echo "Showing differences:" 
            diff "$alg_file" "$hls_file"
            ALL_MATCH=false
        fi
    done
    
    # 输出文件统计信息
    echo -e "\n=== Output File Statistics ==="
    echo "Total file pairs: $TOTAL_FILES"
    echo "Matched file pairs: $MATCHED_FILES"
    
    # 输出文件大小信息（去重）
    echo -e "\nFile sizes:"
    printf "%s\n" "${ALL_FILES[@]}" | sort -u | xargs ls -la 2>/dev/null | awk '{print $5, $9}'
    
    # 输出文件内容预览（只显示存在的文件）
    echo -e "\n=== Output File Content Preview ==="
    for alg_file in "${ALG_FILES[@]}"; do
        pattern="${alg_file#alg_}"
        pattern="${pattern%_output.txt}"
        hls_file="hls_${pattern}_output.txt"
        
        if [ -f "$alg_file" ]; then
            echo -e "\nAlgorithm $pattern output (first 5 lines):"
            head -n 5 "$alg_file"
        fi
        
        if [ -f "$hls_file" ]; then
            echo -e "\nHLS $pattern output (first 5 lines):"
            head -n 5 "$hls_file"
        fi
    done
    
    # 输出总体结果
    echo -e "\n=== Overall Comparison Result ==="
    if [ "$ALL_MATCH" = true ] && [ $TOTAL_FILES -gt 0 ]; then
        echo "✅ All $TOTAL_FILES file pairs match successfully!"
    else
        echo "❌ Some file pairs do not match or are missing."
        echo "Matched: $MATCHED_FILES out of $TOTAL_FILES pairs"
    fi
    
    echo -e "\nOutput comparison completed."
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
        all)
            clean
            build_alg_top
            build_hls_top
            run_alg_top
            run_hls_top
            compare_outputs
            ;;
        clean)
            clean
            ;;
        help)
            show_help
            ;;
        compare)
            compare_outputs
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