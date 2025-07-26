#!/usr/bin/env python3
"""
Random Image Generator
Generates random images with integer data type based on input width, height and format
"""

import numpy as np
import argparse
import os


def generate_random_image(width, height, image_format='RGB', bit_depth=8):
    """
    Generate random image
    
    Args:
        width (int): Image width
        height (int): Image height
        image_format (str): Image format ('RGB', 'YUV', 'BAYER', 'GRAY')
        bit_depth (int): Bit depth (8, 12, 16)
    
    Returns:
        np.ndarray: Generated random image data
    """
    # Determine number of channels based on format
    if image_format.upper() == 'RGB':
        channels = 3
    elif image_format.upper() == 'YUV':
        channels = 3
    elif image_format.upper() == 'BAYER':
        channels = 1
    elif image_format.upper() == 'GRAY' or image_format.upper() == 'GRAYSCALE':
        channels = 1
    else:
        raise ValueError(f"Unsupported image format: {image_format}")
    
    # Determine maximum value based on bit depth
    max_value = (2 ** bit_depth) - 1
    
    # Generate random data
    if channels == 1:
        random_data = np.random.randint(0, max_value + 1, (height, width), dtype=np.int32)
    else:
        random_data = np.random.randint(0, max_value + 1, (height, width, channels), dtype=np.int32)
    
    return random_data


def save_image(image_data, output_path, bit_depth=8):
    """
    Save image to file in specified format
    
    Args:
        image_data (np.ndarray): Image data
        output_path (str): Output file path
        bit_depth (int): Bit depth
    """
    with open(output_path, 'w') as f:
        # Check if it's single channel
        if len(image_data.shape) == 2:
            # Single channel, one pixel data per line
            for row in image_data:
                for pixel in row:
                    f.write(f"{pixel}\n")
        else:
            # Multi-channel, each channel's data is placed in a line, separated by spaces
            height, width, channels = image_data.shape
            for h in range(height):
                for w in range(width):
                    # Write all channel values of current pixel
                    pixel_values = ' '.join(str(image_data[h, w, c]) for c in range(channels))
                    f.write(f"{pixel_values}\n")
    
    print(f"Image saved to: {output_path}")


def main():
    parser = argparse.ArgumentParser(description='Generate random image')
    parser.add_argument('--width', type=int, required=True, help='Image width')
    parser.add_argument('--height', type=int, required=True, help='Image height')
    parser.add_argument('--format', type=str, default='RGB', choices=['RGB', 'YUV', 'BAYER', 'GRAY'], help='Image format')
    parser.add_argument('--bit-depth', type=int, default=8, choices=[8, 12, 16], help='Bit depth')
    parser.add_argument('--output', type=str, default='random_image.png', help='Output file path')
    
    args = parser.parse_args()
    
    # Validate parameters
    if args.width <= 0 or args.height <= 0:
        print("Error: Width and height must be positive integers")
        return
    
    # 生成随机图像
    try:
        random_image = generate_random_image(args.width, args.height, args.format, args.bit_depth)
        
        # Save image
        save_image(random_image, args.output, args.bit_depth)
        
        # Print image information
        print(f"Image information:")
        print(f"  Size: {args.width} x {args.height}")
        print(f"  Format: {args.format}")
        print(f"  Bit depth: {args.bit_depth}-bit")
        print(f"  Data type: {random_image.dtype}")
        print(f"  Data range: [{random_image.min()}, {random_image.max()}]")
        
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
