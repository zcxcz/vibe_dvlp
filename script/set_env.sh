#!/bin/bash

# 环境变量设置脚本
# 此脚本用于设置sync相关脚本所需的路径变量
# 使用方法：source set_env.sh

# 项目根目录
PRJ_NAME="vibe_crop_0817_v1"
LINUX_PRJ_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WIN_PRJ_ROOT="/mnt/f/trunk_prj/hls_project/${PRJ_NAME}"
WIN_HLS_PRJ_DIR="${WIN_PRJ_ROOT}/hls"
WIN_VIVADO_PRJ_DIR="${WIN_PRJ_ROOT}/vivado/"

# 设置sync_c2rtl.sh所需的路径
# 源目录（默认为项目根目录）
SYNC_C2RTL_SOURCE_DIR="$LINUX_PRJ_ROOT"
# 目标目录（根据实际情况修改）
SYNC_C2RTL_TARGET_DIR="$WIN_HLS_PRJ_DIR"

# 设置sync_rtl2pynq.sh所需的路径
# 源路径（根据实际情况修改）
SYNC_RTL2PYNQ_SOURCE_DIR="$WIN_VIVADO_PRJ_DIR"
# 目标路径（根据实际情况修改）
SYNC_RTL2PYNQ_DEST_DIR="$LINUX_PRJ_ROOT"
# 目标目录名称
SYNC_RTL2PYNQ_DEST_DIR_NAME="$(basename $LINUX_PRJ_ROOT)"

# 提示信息
echo "环境变量已设置："
echo "- LINUX_PRJ_ROOT: $LINUX_PRJ_ROOT"
echo "- SYNC_C2RTL_SOURCE_DIR: $SYNC_C2RTL_SOURCE_DIR"
echo "- SYNC_C2RTL_TARGET_DIR: $SYNC_C2RTL_TARGET_DIR"
echo "- SYNC_RTL2PYNQ_SOURCE_DIR: $SYNC_RTL2PYNQ_SOURCE_DIR"
echo "- SYNC_RTL2PYNQ_DEST_DIR: $SYNC_RTL2PYNQ_DEST_DIR"
echo "- SYNC_RTL2PYNQ_DEST_DIR_NAME: $SYNC_RTL2PYNQ_DEST_DIR_NAME"
echo ""
echo "使用方法："
echo "1. 如需修改默认路径，请编辑此脚本"
echo "2. 运行sync脚本时："
echo "   ./sync_c2rtl.sh $SYNC_C2RTL_SOURCE_DIR $SYNC_C2RTL_TARGET_DIR"
echo "   ./sync_rtl2pynq.sh $SYNC_RTL2PYNQ_SOURCE_DIR $SYNC_RTL2PYNQ_DEST_DIR $SYNC_RTL2PYNQ_DEST_DIR_NAME"