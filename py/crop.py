"""
File: crop.py
Description: Fixed crop function with correct pixel parsing and coordinate handling
Author: Fixed for HLS integration
------------------------------------------------------------
"""

import numpy as np
import sys
import os


def crop(input_file, output_file, start_x, start_y, end_x, end_y, crop_enable, image_width, image_height, bit_depth=8, bayer_pattern="rggb"):
    """
    Crop function with correct coordinate-based cropping
    
    Parameters:
    ----------
    input_file: str
        Path to input image file (text format, one pixel per line)
    output_file: str
        Path to output image file (text format, one pixel per line)
    start_x: int
        Starting x coordinate (inclusive)
    start_y: int
        Starting y coordinate (inclusive)
    end_x: int
        Ending x coordinate (inclusive)
    end_y: int
        Ending y coordinate (inclusive)
    crop_enable: bool
        Whether to enable cropping operation
    image_width: int
        Width of the input image in pixels
    image_height: int
        Height of the input image in pixels
    bit_depth: int
        Bit depth of the image data (default: 8)
    bayer_pattern: str
        Bayer pattern of the image (default: "rggb")
    
    Returns:
    -------
    bool: True if operation successful, False otherwise
    """
    
    if not crop_enable:
        print("Crop operation disabled")
        # If disabled, just copy the input to output
        try:
            with open(input_file, 'r') as f_in, open(output_file, 'w') as f_out:
                for line in f_in:
                    f_out.write(line)
            return True
        except Exception as e:
            print(f"Error copying file: {e}")
            return False
    
    try:
        # Read input file and parse pixel values
        with open(input_file, 'r') as f:
            pixel_lines = [line.strip() for line in f if line.strip()]
        
        # Convert to integers
        pixels = [int(line) for line in pixel_lines]
        total_pixels = len(pixels)
        
        # Validate image dimensions
        expected_pixels = image_width * image_height
        if total_pixels != expected_pixels:
            print(f"Error: Image size mismatch. Expected {expected_pixels} pixels for {image_width}x{image_height}, got {total_pixels}")
            return False
        
        # Reshape to 2D array
        input_2d = np.array(pixels).reshape((image_height, image_width))
        
        # Validate coordinates
        if start_x < 0 or start_x >= image_width:
            print(f"Error: Invalid start_x coordinate {start_x}. Must be 0 <= start_x < {image_width}")
            return False
        
        if start_y < 0 or start_y >= image_height:
            print(f"Error: Invalid start_y coordinate {start_y}. Must be 0 <= start_y < {image_height}")
            return False
        
        if end_x < 0 or end_x >= image_width:
            print(f"Error: Invalid end_x coordinate {end_x}. Must be 0 <= end_x < {image_width}")
            return False
        
        if end_y < 0 or end_y >= image_height:
            print(f"Error: Invalid end_y coordinate {end_y}. Must be 0 <= end_y < {image_height}")
            return False
        
        if start_x > end_x or start_y > end_y:
            print(f"Error: Invalid crop coordinates ({start_x},{start_y}) to ({end_x},{end_y}). Start must be <= end")
            return False
        
        # Perform cropping using coordinates (inclusive end)
        cropped = input_2d[start_y:end_y+1, start_x:end_x+1]
        
        # Flatten and save with one pixel per line
        cropped_pixels = cropped.flatten()
        with open(output_file, 'w') as f:
            for pixel in cropped_pixels:
                f.write(f"{pixel}\n")
        
        cropped_width = end_x - start_x + 1
        cropped_height = end_y - start_y + 1
        print(f"Crop successful: {image_width}x{image_height} -> {cropped_width}x{cropped_height}")
        print(f"Cropped region: ({start_x},{start_y}) to ({end_x},{end_y})")
        print(f"Cropped image saved to: {output_file}")
        print(f"Output file size: {len(cropped_pixels)} pixels")
        
        return True
        
    except Exception as e:
        print(f"Error during crop operation: {e}")
        import traceback
        traceback.print_exc()
        return False


def main():
    """
    Main function for command line usage
    """
    if len(sys.argv) < 10:
        print("Usage: python crop.py <input_file> <output_file> <start_x> <start_y> <end_x> <end_y> <crop_enable> <image_width> <image_height> <bit_depth> [bayer_pattern]")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    start_x = int(sys.argv[3])
    start_y = int(sys.argv[4])
    end_x = int(sys.argv[5])
    end_y = int(sys.argv[6])
    crop_enable = sys.argv[7].lower() == 'true'
    image_width = int(sys.argv[8])
    image_height = int(sys.argv[9])
    bit_depth = int(sys.argv[10])
    bayer_pattern = sys.argv[11] if len(sys.argv) > 11 else "rggb"
    
    success = crop(input_file, output_file, start_x, start_y, end_x, end_y, crop_enable, image_width, image_height, bit_depth, bayer_pattern)
    
    if success:
        print("Crop operation completed successfully")
    else:
        print("Crop operation failed")
        sys.exit(1)


if __name__ == "__main__":
    main()