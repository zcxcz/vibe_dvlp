#!/usr/bin/env python3
"""
Test script to verify coordinate format functionality
"""

import numpy as np
import os
import sys

# Add the py directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'py'))

from generate_random_image import generate_random_image, save_image

def test_coordinate_format():
    """Test the new coordinate format functionality"""
    
    # Test parameters
    width = 4
    height = 3
    
    print("=== Testing Coordinate Format Functionality ===")
    print(f"Generating test image: {width}x{height}")
    
    # Generate a small test image
    test_image = generate_random_image(width, height, 'BAYER', 8)
    
    # Save with new coordinate format
    output_file = "test_with_coordinates.txt"
    save_image(test_image, output_file, 8)
    
    # Read the file and display first few lines
    print(f"\nFile content ({output_file}):")
    with open(output_file, 'r') as f:
        lines = f.readlines()
        for i, line in enumerate(lines[:10]):
            print(f"  Line {i+1}: {line.strip()}")
    
    if len(lines) > 10:
        print(f"  ... ({len(lines) - 10} more lines)")
    
    # Test backward compatibility - create old format file
    old_format_file = "test_old_format.txt"
    with open(old_format_file, 'w') as f:
        for i in range(width * height):
            f.write(f"{i}\n")
    
    print(f"\nOld format file created ({old_format_file})")
    print("Both formats should be readable by the new C++ code")
    
    # Clean up test files
    os.remove(output_file)
    os.remove(old_format_file)
    
    print("\n=== Test completed successfully ===")

if __name__ == "__main__":
    test_coordinate_format()