#!/bin/bash

# 同步脚本：将Vivado工程中的bit和hwh文件同步到项目bit目录
# 使用方法：./sync_rtl2pynq.sh

# 尝试加载环境变量设置脚本
if [ -f "$(dirname "$0")/set_env.sh" ]; then
    source "$(dirname "$0")/set_env.sh"
else
    echo "错误：无法找到环境变量设置脚本 set_env.sh"
    echo "请确保该脚本存在于当前目录或正确设置路径"
    exit 1
fi

# 检查环境变量是否已设置
if [ -z "$WIN_VIVADO_PRJ_DIR" ] || [ -z "$WIN_PRJ_ROOT" ]; then
    echo "错误：必要的环境变量未设置，请检查set_env.sh脚本"
    exit 1
fi

# 设置源目录和目标目录
SOURCE_DIR="$WIN_VIVADO_PRJ_DIR"
TARGET_DIR="$WIN_PRJ_ROOT/bit"

# 创建目标目录（如果不存在）
echo "创建目标目录 $TARGET_DIR..."
mkdir -p "$TARGET_DIR"
if [ $? -ne 0 ]; then
    echo "错误：创建目标目录 $TARGET_DIR 失败"
    exit 1
fi

# 检查源目录是否存在
if [ ! -d "$SOURCE_DIR" ]; then
    echo "错误：源目录 $SOURCE_DIR 不存在"
    exit 1
fi

# 查找并复制.bit文件
echo "查找并复制.bit文件..."
BITLE_FILES=$(find "$SOURCE_DIR" -name "*.bit")
if [ -z "$BITLE_FILES" ]; then
    echo "警告：在 $SOURCE_DIR 中未找到.bit文件"
else
    for file in $BITLE_FILES; do
        echo "复制 $file 到 $TARGET_DIR..."
        cp -f "$file" "$TARGET_DIR/${PRJ_NAME}.bit"
        if [ $? -ne 0 ]; then
            echo "错误：复制 $file 失败"
            exit 1
        fi
    done
fi

# 查找并复制.hwh文件
echo "查找并复制.hwh文件..."
HWH_FILES=$(find "$SOURCE_DIR" -name "*.hwh")
if [ -z "$HWH_FILES" ]; then
    echo "警告：在 $SOURCE_DIR 中未找到.hwh文件"
else
    for file in $HWH_FILES; do
        echo "复制 $file 到 $TARGET_DIR..."
        cp -f "$file" "$TARGET_DIR/${PRJ_NAME}.hwh"
        if [ $? -ne 0 ]; then
            echo "错误：复制 $file 失败"
            exit 1
        fi
    done
fi

echo "同步完成！"
echo "- 源目录: $SOURCE_DIR"
echo "- 目标目录: $TARGET_DIR"
exit 0