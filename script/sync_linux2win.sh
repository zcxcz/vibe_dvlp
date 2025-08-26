#!/bin/bash

# 同步脚本：将Linux工程目录文件同步到Windows工程目录
# 使用方法：./sync_linux2win.sh

# 尝试加载环境变量设置脚本
if [ -f "$(dirname "$0")/set_env.sh" ]; then
    source "$(dirname "$0")/set_env.sh"
else
    echo "错误：无法找到环境变量设置脚本 set_env.sh"
    echo "请确保该脚本存在于当前目录或正确设置路径"
    exit 1
fi

# 检查环境变量是否已设置
if [ -z "$LINUX_PRJ_ROOT" ] || [ -z "$WIN_PRJ_ROOT" ] || [ -z "$WIN_HLS_PRJ_DIR" ] || [ -z "$WIN_VIVADO_PRJ_DIR" ]; then
    echo "错误：必要的环境变量未设置，请检查set_env.sh脚本"
    exit 1
fi

# 创建Windows工程目录（如果不存在）
echo "创建Windows工程目录..."
mkdir -p "$WIN_HLS_PRJ_DIR"
mkdir -p "$WIN_VIVADO_PRJ_DIR"

# 同步文件
# 注意：这里使用rsync命令同步文件，--delete选项会删除目标目录中不存在于源目录的文件
# 排除不需要同步的目录和文件
EXCLUDE_LIST=("--exclude=.git" "--exclude=build" "--exclude=log" "--exclude=*.bit" "--exclude=*.hwh")

# 同步整个工程目录到Windows目录
echo "正在同步Linux工程目录到Windows目录..."
rsync -avzh "$LINUX_PRJ_ROOT/" "$WIN_PRJ_ROOT" "${EXCLUDE_LIST[@]}"

# 同步特定文件到Vivado目录（如果需要）
# 这里可以添加额外的同步命令

echo "同步完成！"
echo "- Linux工程目录: $LINUX_PRJ_ROOT"
echo "- Windows工程目录: $WIN_PRJ_ROOT"
