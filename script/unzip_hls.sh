#!/bin/bash

# 解压脚本：将HLS工程的IP文件解压到指定目录
# 使用方法：./unzip_hls.sh

# 尝试加载环境变量设置脚本
if [ -f "$(dirname "$0")/set_env.sh" ]; then
    source "$(dirname "$0")/set_env.sh"
else
    echo "错误：无法找到环境变量设置脚本 set_env.sh"
    echo "请确保该脚本存在于当前目录或正确设置路径"
    exit 1
fi

# 检查环境变量是否已设置
if [ -z "$WIN_HLS_PRJ_DIR" ]; then
    echo "错误：WIN_HLS_PRJ_DIR环境变量未设置，请检查set_env.sh脚本"
    exit 1
fi

# 源路径（转换为Linux格式）
# 默认为Windows HLS工程目录下的ip文件夹
SOURCE_DIR="${WIN_HLS_PRJ_DIR}"

# 检查源目录是否存在
if [ ! -d "$SOURCE_DIR" ]; then
    echo "警告：源目录 $SOURCE_DIR 不存在，尝试创建..."
    mkdir -p "$SOURCE_DIR"
    if [ $? -ne 0 ]; then
        echo "错误：创建源目录 $SOURCE_DIR 失败"
        exit 1
    fi
fi

# 检查是否有.zip文件
ZIP_FILES=$(find "$SOURCE_DIR" -name "*.zip")
if [ -z "$ZIP_FILES" ]; then
    echo "错误：在 $SOURCE_DIR 中未找到.zip文件"
    exit 1
fi

# 解压所有.zip文件到目标目录
echo "开始解压文件..."
# 确保目标目录存在
TARGET_DIR="$WIN_HLS_PRJ_DIR/solution1/impl/ip"
mkdir -p "$TARGET_DIR"
if [ $? -ne 0 ]; then
    echo "错误：创建目标目录 $TARGET_DIR 失败"
    exit 1
fi

for file in $ZIP_FILES; do
    echo "解压 $file 到 $TARGET_DIR..."
    # 创建与zip文件同名的目录
    DEST_DIR="$TARGET_DIR/$(basename $file .zip)"
    mkdir -p "$DEST_DIR"
    echo "创建目标目录 $DEST_DIR..."
    if [ $? -ne 0 ]; then
        echo "错误：创建目标目录 $DEST_DIR 失败"
        exit 1
    fi
    unzip -o "$file" -d "$DEST_DIR"
    if [ $? -ne 0 ]; then
        echo "错误：解压 $file 失败"
        exit 1
    fi
done

echo "所有文件解压完成"
exit 0