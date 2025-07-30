#!/usr/bin/env python3
"""
Crop adapter for vibe project
适配器脚本，用于从命令行调用crop.py功能
"""

import sys
import os
import numpy as np
from pathlib import Path

# 添加Inf目录到路径
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'Inf'))

try:
    from Inf.crop.crop import Crop
except ImportError as e:
    print(f"Error importing crop module: {e}")
    sys.exit(1)

def load_raw_image(file_path, width, height, bit_depth):
    """加载RAW图像数据"""
    try:
        # 读取RAW数据
        data = np.fromfile(file_path, dtype=np.uint8 if bit_depth <= 8 else np.uint16)
        
        # 重塑为2D数组
        if len(data) == width * height:
            return data.reshape((height, width))
        else:
            print(f"Warning: File size {len(data)} doesn't match expected {width*height}")
            # 截断或填充
            expected_size = width * height
            if len(data) > expected_size:
                data = data[:expected_size]
            else:
                data = np.pad(data, (0, expected_size - len(data)), 'constant')
            return data.reshape((height, width))
    except Exception as e:
        print(f"Error loading image: {e}")
        sys.exit(1)

def save_raw_image(image_data, file_path):
    """保存RAW图像数据"""
    try:
        image_data.astype(np.uint8).tofile(file_path)
        print(f"Saved cropped image to: {file_path}")
    except Exception as e:
        print(f"Error saving image: {e}")
        sys.exit(1)

def main():
    """主函数"""
    if len(sys.argv) != 10:
        print("Usage: python crop_adapter.py <input_file> <output_file> <width> <height> <crop_width> <crop_height> <crop_enable> <bit_depth> <bayer_pattern>")
        sys.exit(1)
    
    # 解析参数
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    width = int(sys.argv[3])
    height = int(sys.argv[4])
    crop_width = int(sys.argv[5])
    crop_height = int(sys.argv[6])
    crop_enable = int(sys.argv[7])
    bit_depth = int(sys.argv[8])
    bayer_pattern = sys.argv[9]
    
    # 加载输入图像
    img = load_raw_image(input_file, width, height, bit_depth)
    
    # 配置参数
    sensor_info = {
        "width": width,
        "height": height,
        "bit_depth": bit_depth,
        "bayer_pattern": bayer_pattern
    }
    
    platform = {
        "in_file": input_file,
        "is_save": True
    }
    
    parm_cro = {
        "new_width": crop_width,
        "new_height": crop_height,
        "is_enable": bool(crop_enable),
        "is_debug": True,
        "is_save": True
    }
    
    # 执行裁剪
    crop_processor = Crop(img, platform, sensor_info, parm_cro)
    cropped_img = crop_processor.execute()
    
    # 保存结果
    save_raw_image(cropped_img, output_file)
    
    print(f"Crop processing completed:")
    print(f"  Input: {input_file} ({width}x{height})")
    print(f"  Output: {output_file} ({crop_width}x{crop_height})")
    print(f"  Crop enabled: {bool(crop_enable)}")

if __name__ == "__main__":
    main()