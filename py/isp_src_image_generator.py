#!/usr/bin/env python3
"""
ISP Source Image Generator
根据配置生成随机图像数据或处理现有图像
支持 RAW, RGB, YUV 格式
"""

import json
import os
import sys
import random
from datetime import datetime

class ISPSrcImageGenerator:
    def __init__(self, config_path):
        self.config_path = config_path
        self.config = None
        self.image_width = 0
        self.image_height = 0
        self.src_image_format = ""
        self.src_image_bitwidth = 8
        self.generate_random_enable = False
        self.src_image_path = ""
        self.random_src_image_path = ""
        
    def load_config(self):
        """加载图像配置文件"""
        try:
            with open(self.config_path, 'r') as f:
                self.config = json.load(f)
            
            # 提取关键配置
            image_format = self.config.get("image_format", {})
            image_pattern = self.config.get("image_pattern", {})
            
            self.src_image_format = image_format.get("src_image_format", "BAYER")
            self.src_image_bitwidth = image_format.get("src_image_bitwidth", 8)
            self.generate_random_enable = bool(image_format.get("generate_random_src_image_enable", 0))
            
            self.src_image_path = image_pattern.get("src_image_path", "data/src_image.txt")
            self.random_src_image_path = image_pattern.get("random_src_image_path", "data/src_image_random_generate.txt")
            
            print(f"配置加载成功:")
            print(f"  图像格式: {self.src_image_format}")
            print(f"  位宽: {self.src_image_bitwidth}")
            print(f"  生成随机图像: {self.generate_random_enable}")
            print(f"  输出路径: {self.random_src_image_path}")
            
            return True
            
        except Exception as e:
            print(f"加载配置文件失败: {e}")
            return False
    
    def get_data_range(self):
        """根据位宽获取数据范围"""
        if self.src_image_bitwidth <= 8:
            max_val = (1 << self.src_image_bitwidth) - 1
            return 0, max_val
        elif self.src_image_bitwidth <= 16:
            max_val = (1 << self.src_image_bitwidth) - 1
            return 0, max_val
        else:
            return 0, 65535  # 默认16位范围
    
    def format_pixel_data(self, value, row, col):
        """根据格式格式化像素数据"""
        min_val, max_val = self.get_data_range()
        
        # 确保值在有效范围内
        value = max(min_val, min(max_val, value))
        
        # 根据位宽格式化
        if self.src_image_bitwidth <= 8:
            hex_str = f"{value:02x}"
        elif self.src_image_bitwidth <= 16:
            hex_str = f"{value:04x}"
        else:
            hex_str = f"{value:04x}"
        
        # 添加坐标注释
        return f"{hex_str}  # ({col:4d}, {row:4d})"
    
    def generate_raw_bayer_data(self, width, height):
        """生成BAYER格式的RAW数据"""
        print(f"生成BAYER格式数据: {width}x{height}")
        data = []
        min_val, max_val = self.get_data_range()
        
        for row in range(height):
            for col in range(width):
                # 生成随机像素值
                value = random.randint(min_val, max_val)
                formatted = self.format_pixel_data(value, row, col)
                data.append(formatted)
        
        return data
    
    def generate_rgb_data(self, width, height):
        """生成RGB格式数据"""
        print(f"生成RGB格式数据: {width}x{height}")
        data = []
        min_val, max_val = self.get_data_range()
        
        for row in range(height):
            for col in range(width):
                # 生成RGB三个分量
                r = random.randint(min_val, max_val)
                g = random.randint(min_val, max_val)
                b = random.randint(min_val, max_val)
                
                # 根据位宽格式化
                if self.src_image_bitwidth <= 8:
                    hex_str = f"{r:02x}{g:02x}{b:02x}"
                else:
                    hex_str = f"{r:04x}{g:04x}{b:04x}"
                
                formatted = f"{hex_str}  # ({col:4d}, {row:4d})"
                data.append(formatted)
        
        return data
    
    def generate_yuv_data(self, width, height):
        """生成YUV格式数据"""
        print(f"生成YUV格式数据: {width}x{height}")
        data = []
        min_val, max_val = self.get_data_range()
        
        for row in range(height):
            for col in range(width):
                # 生成YUV三个分量 (Y:亮度, U/V:色度)
                y = random.randint(min_val, max_val)  # 亮度
                u = random.randint(min_val, max_val)  # 色度U
                v = random.randint(min_val, max_val)  # 色度V
                
                # 根据位宽格式化
                if self.src_image_bitwidth <= 8:
                    hex_str = f"{y:02x}{u:02x}{v:02x}"
                else:
                    hex_str = f"{y:04x}{u:04x}{v:04x}"
                
                formatted = f"{hex_str}  # ({col:4d}, {row:4d})"
                data.append(formatted)
        
        return data
    
    def generate_random_image_data(self, width=32, height=32):
        """根据配置生成随机图像数据"""
        print(f"开始生成随机图像数据: {width}x{height}")
        print(f"格式: {self.src_image_format}, 位宽: {self.src_image_bitwidth}")
        
        if self.src_image_format.upper() == "BAYER":
            return self.generate_raw_bayer_data(width, height)
        elif self.src_image_format.upper() == "RGB":
            return self.generate_rgb_data(width, height)
        elif self.src_image_format.upper() in ["YUV", "YUV422", "YUV420"]:
            return self.generate_yuv_data(width, height)
        else:
            print(f"警告: 不支持的格式 {self.src_image_format}，使用BAYER格式")
            return self.generate_raw_bayer_data(width, height)
    
    def save_image_data(self, data, output_path):
        """保存图像数据到文件"""
        try:
            # 确保输出目录存在
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            
            with open(output_path, 'w') as f:
                # 写入文件头信息
                f.write(f"# ISP Source Image Data\n")
                f.write(f"# Format: {self.src_image_format}\n")
                f.write(f"# Bitwidth: {self.src_image_bitwidth}\n")
                f.write(f"# Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
                f.write(f"# Total pixels: {len(data)}\n")
                f.write("#\n")
                
                # 写入像素数据
                for line in data:
                    f.write(line + "\n")
            
            print(f"图像数据已保存到: {output_path}")
            print(f"总像素数: {len(data)}")
            return True
            
        except Exception as e:
            print(f"保存文件失败: {e}")
            return False
    
    def process(self, width=32, height=32):
        """主处理流程"""
        print("=== ISP Source Image Generator ===")
        
        # 加载配置
        if not self.load_config():
            return False
        
        # 检查是否需要生成随机图像
        if not self.generate_random_enable:
            print("随机图像生成已禁用，跳过生成过程")
            return True
        
        # 生成随机图像数据
        image_data = self.generate_random_image_data(width, height)
        
        if not image_data:
            print("生成图像数据失败")
            return False
        
        # 保存到文件
        output_path = os.path.join("..", self.random_src_image_path)
        return self.save_image_data(image_data, output_path)

def main():
    # 默认配置
    config_path = "../src/image_config.json"
    
    # 检查命令行参数
    if len(sys.argv) > 1:
        config_path = sys.argv[1]
    
    # 图像尺寸参数
    width = 32
    height = 32
    if len(sys.argv) > 3:
        width = int(sys.argv[2])
        height = int(sys.argv[3])
    
    # 创建生成器并运行
    generator = ISPSrcImageGenerator(config_path)
    success = generator.process(width, height)
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())