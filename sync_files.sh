#!/bin/bash

# 同步脚本：将源文件和配置文件同步到目标目录
# 使用方法：./sync_files.sh <源目录> <目标目录>

# 检查参数数量
if [ $# -ne 2 ]; then
    echo "用法: $0 <源目录> <目标目录>"
    echo "示例: $0 /home/sheldon/hls_project/vibe_crop /home/sheldon/hls_project/target_dir"
    exit 1
fi

# 获取参数
SOURCE_DIR="$1"
TARGET_DIR="$2"

# 检查源目录是否存在
if [ ! -d "$SOURCE_DIR" ]; then
    echo "错误：源目录 $SOURCE_DIR 不存在"
    exit 1
fi

# 检查源目录是否存在src子目录
if [ ! -d "$SOURCE_DIR/src" ]; then
    echo "错误：源目录 $SOURCE_DIR/src 不存在"
    exit 1
fi

# 创建目标目录（如果不存在）
mkdir -p "$TARGET_DIR"

# 同步src目录下的.cpp文件
echo "正在同步.cpp文件..."
if [ -d "$SOURCE_DIR/src" ]; then
    mkdir -p "$TARGET_DIR/src"
    cp "$SOURCE_DIR/src"/*.cpp "$TARGET_DIR/src/" 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "✓ 成功同步.cpp文件到 $TARGET_DIR/src/"
        ls -la "$TARGET_DIR/src"/*.cpp 2>/dev/null || echo "  注意：未找到.cpp文件"
    else
        echo "⚠ 警告：同步.cpp文件时出现问题"
        exit 1
    fi
else
    echo "⚠ 警告：源目录中未找到src子目录"
fi

# 同步src目录下的.h文件
echo "正在同步.h文件..."
if [ -d "$SOURCE_DIR/src" ]; then
    mkdir -p "$TARGET_DIR/src"
    cp "$SOURCE_DIR/src"/*.h "$TARGET_DIR/src/" 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "✓ 成功同步.h文件到 $TARGET_DIR/src/"
        ls -la "$TARGET_DIR/src"/*.h 2>/dev/null || echo "  注意：未找到.h文件"
    else
        echo "⚠ 警告：同步.h文件时出现问题"
        exit 1
    fi
else
    echo "⚠ 警告：源目录中未找到src子目录"
fi

# 同步.json配置文件
echo "正在同步.json配置文件..."
JSON_FILES=$(find "$SOURCE_DIR" -maxdepth 1 -name "*.json" 2>/dev/null)
if [ -n "$JSON_FILES" ]; then
    cp "$SOURCE_DIR"/data/*.json "$TARGET_DIR/data/" 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "✓ 成功同步.json文件到 $TARGET_DIR/data/"
        ls -la "$TARGET_DIR"/data/*.json 2>/dev/null || echo "  注意：未找到.json文件"
    else
        echo "⚠ 警告：同步.json文件时出现问题"
        exit 1
    fi
else
    echo "⚠ 警告：源目录中未找到.json文件"
fi

# 显示同步结果
echo ""
echo "=== 同步完成 ==="
echo "源目录: $SOURCE_DIR"
echo "目标目录: $TARGET_DIR"
echo ""

echo "已同步的文件："
if [ -d "$TARGET_DIR/src" ]; then
    echo "源文件(.cpp):"
    ls -la "$TARGET_DIR/src"/*.cpp 2>/dev/null || echo "  无.cpp文件"
fi

if [ -d "$TARGET_DIR/src" ]; then
    echo "源文件(.h):"
    ls -la "$TARGET_DIR/src"/*.h 2>/dev/null || echo "  无.h文件"
fi

if [ -d "$TARGET_DIR/data" ]; then
    echo "配置文件(.json):"
    ls -la "$TARGET_DIR/data"/*.json 2>/dev/null || echo "  无.json文件"
fi

echo ""
echo "同步操作完成！"