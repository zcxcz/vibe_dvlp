#!/usr/bin/env python3
"""
ISP Source Image Generator
Generate random image data or process existing images based on configuration
Supports RAW, RGB, YUV formats
"""

import json
import os
import sys
import random
from datetime import datetime

class ISPImageGenarator:
    def __init__(self, config_path):
        self.config_path = config_path
        self.config = None
        self.image_width = 0
        self.image_height = 0
        self.src_image_format = ""
        self.src_image_data_bitwidth = 8
        self.generate_random_enable = False
        self.src_image_path = ""
        self.random_src_image_path = ""
        
    def load_config(self):
        """Load image configuration file"""
        try:
            with open(self.config_path, 'r') as f:
                self.config = json.load(f)
            
            # Extract key configurations
            image_format = self.config.get("image_format", {})
            image_pattern = self.config.get("image_pattern", {})
            
            self.src_image_format = image_format.get("src_image_format", "BAYER")
            self.src_image_data_bitwidth = image_format.get("src_image_data_bitwidth", 8)
            self.generate_random_enable = bool(image_format.get("generate_random_src_image_enable", 0))
            
            self.src_image_path = image_pattern.get("src_image_path", "data/src_image.txt")
            self.random_src_image_path = image_pattern.get("random_src_image_path", "data/src_image_random_generate.txt")
            
            print(f"Configuration loaded successfully:")
            print(f"  Image format: {self.src_image_format}")
            print(f"  Bit width: {self.src_image_data_bitwidth}")
            print(f"  Generate random image: {self.generate_random_enable}")
            print(f"  Output path: {self.random_src_image_path}")
            
            return True
            
        except Exception as e:
            print(f"Failed to load configuration file: {e}")
            return False
    
    def get_data_range(self):
        """Get data range based on bit width"""
        if self.src_image_data_bitwidth <= 8:
            max_val = (1 << self.src_image_data_bitwidth) - 1
            return 0, max_val
        elif self.src_image_data_bitwidth <= 16:
            max_val = (1 << self.src_image_data_bitwidth) - 1
            return 0, max_val
        else:
            return 0, 65535  # Default 16-bit range
    
    def format_pixel_data(self, value, row, col):
        """Format pixel data according to format"""
        min_val, max_val = self.get_data_range()
        
        # Ensure value is within valid range
        value = max(min_val, min(max_val, value))
        
        # Format according to bit width
        if self.src_image_data_bitwidth <= 8:
            hex_str = f"{value:02x}"
        elif self.src_image_data_bitwidth <= 16:
            hex_str = f"{value:04x}"
        else:
            hex_str = f"{value:04x}"
        
        # Add coordinate annotation
        return f"{hex_str}  # ({col:4d}, {row:4d})"
    
    def generate_raw_bayer_data(self, width, height):
        """Generate RAW data in BAYER format"""
        print(f"Generating BAYER format data: {width}x{height}")
        data = []
        min_val, max_val = self.get_data_range()
        
        for row in range(height):
            for col in range(width):
                # Generate random pixel value
                value = random.randint(min_val, max_val)
                formatted = self.format_pixel_data(value, row, col)
                data.append(formatted)
        
        return data
    
    def generate_rgb_data(self, width, height):
        """Generate RGB format data"""
        print(f"Generating RGB format data: {width}x{height}")
        data = []
        min_val, max_val = self.get_data_range()
        
        for row in range(height):
            for col in range(width):
                # Generate RGB three components
                r = random.randint(min_val, max_val)
                g = random.randint(min_val, max_val)
                b = random.randint(min_val, max_val)
                
                # Format according to bit width
                if self.src_image_data_bitwidth <= 8:
                    hex_str = f"{r:02x}{g:02x}{b:02x}"
                else:
                    hex_str = f"{r:04x}{g:04x}{b:04x}"
                
                formatted = f"{hex_str}  # ({col:4d}, {row:4d})"
                data.append(formatted)
        
        return data
    
    def generate_yuv_data(self, width, height):
        """Generate YUV format data"""
        print(f"Generating YUV format data: {width}x{height}")
        data = []
        min_val, max_val = self.get_data_range()
        
        for row in range(height):
            for col in range(width):
                # Generate YUV three components (Y:Luminance, U/V:Chrominance)
                y = random.randint(min_val, max_val)  # Luminance
                u = random.randint(min_val, max_val)  # Chrominance U
                v = random.randint(min_val, max_val)  # Chrominance V
                
                # Format according to bit width
                if self.src_image_data_bitwidth <= 8:
                    hex_str = f"{y:02x}{u:02x}{v:02x}"
                else:
                    hex_str = f"{y:04x}{u:04x}{v:04x}"
                
                formatted = f"{hex_str}  # ({col:4d}, {row:4d})"
                data.append(formatted)
        
        return data
    
    def generate_random_image_data(self, width=32, height=32):
        """Generate random image data based on configuration"""
        print(f"Starting to generate random image data: {width}x{height}")
        print(f"Format: {self.src_image_format}, Bit width: {self.src_image_data_bitwidth}")
        
        if self.src_image_format.upper() == "BAYER":
            return self.generate_raw_bayer_data(width, height)
        elif self.src_image_format.upper() == "RGB":
            return self.generate_rgb_data(width, height)
        elif self.src_image_format.upper() in ["YUV", "YUV422", "YUV420"]:
            return self.generate_yuv_data(width, height)
        else:
            print(f"Warning: Unsupported format {self.src_image_format}, using BAYER format")
            return self.generate_raw_bayer_data(width, height)
    
    def save_image_data(self, data, output_path):
        """Save image data to file"""
        try:
            # Ensure output directory exists
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            
            with open(output_path, 'w') as f:
                # Write file header information
                f.write(f"# ISP Source Image Data\n")
                f.write(f"# Format: {self.src_image_format}\n")
                f.write(f"# Bitwidth: {self.src_image_data_bitwidth}\n")
                f.write(f"# Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
                f.write(f"# Total pixels: {len(data)}\n")
                f.write("#\n")
                
                # Write pixel data
                for line in data:
                    f.write(line + "\n")
            
            print(f"Image data saved to: {output_path}")
            print(f"Total pixels: {len(data)}")
            return True
            
        except Exception as e:
            print(f"Failed to save file: {e}")
            return False
    
    def process(self, width=32, height=32):
        """Main processing flow"""
        print("=== ISP Source Image Generator ===")
        
        # Load configuration
        if not self.load_config():
            return False
        
        # Check if random image generation is needed
        if not self.generate_random_enable:
            print("Random image generation is disabled, skipping generation process")
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
    
    # 解析命令行参数
    import argparse
    parser = argparse.ArgumentParser(description='ISP Source Image Generator')
    parser.add_argument('--config', help='Path to configuration file')
    parser.add_argument('config_pos', nargs='?', help='Configuration file path (positional)')
    parser.add_argument('width', nargs='?', type=int, default=32, help='Image width')
    parser.add_argument('height', nargs='?', type=int, default=32, help='Image height')
    
    args = parser.parse_args()
    
    # 优先使用--config参数，否则使用位置参数
    if args.config:
        config_path = args.config
    elif args.config_pos:
        config_path = args.config_pos
    
    width = args.width
    height = args.height
    
    # 创建生成器并运行
    generator = ISPImageGenarator(config_path)
    success = generator.process(width, height)
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())