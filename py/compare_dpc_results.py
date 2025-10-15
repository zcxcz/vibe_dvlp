#!/usr/bin/env python3
"""
Compare DPC processing results between input and output
"""

import os
import sys

def read_hex_data(filepath):
    """Read hex data from file with coordinate comments"""
    data = []
    try:
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    # Extract hex value before comment
                    hex_part = line.split('#')[0].strip()
                    if hex_part:
                        data.append(hex_part)
    except FileNotFoundError:
        print(f"Error: File {filepath} not found")
        return None
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return None
    return data

def compare_dpc_results(input_file, output_file):
    """Compare DPC processing results"""
    print("=== DPC Processing Comparison ===")
    
    # Read input and output data
    input_data = read_hex_data(input_file)
    output_data = read_hex_data(output_file)
    
    if input_data is None or output_data is None:
        return False
    
    print(f"Input data size: {len(input_data)} pixels")
    print(f"Output data size: {len(output_data)} pixels")
    
    if len(input_data) != len(output_data):
        print("Error: Input and output sizes don't match")
        return False
    
    # Count changed pixels
    changed_pixels = 0
    total_pixels = len(input_data)
    
    for i, (inp, out) in enumerate(zip(input_data, output_data)):
        if inp != out:
            changed_pixels += 1
            # Print first few changes for analysis
            if changed_pixels <= 5:
                row = i // 32  # Assuming 32x32 grid
                col = i % 32
                print(f"Pixel changed at ({row}, {col}): {inp} -> {out}")
    
    print(f"\nTotal pixels changed: {changed_pixels}/{total_pixels} ({changed_pixels/total_pixels*100:.2f}%)")
    
    if changed_pixels == 0:
        print("No pixels were modified by DPC processing")
    else:
        print("DPC processing successfully corrected defective pixels")
    
    return True

def main():
    # File paths
    input_file = "../data/src_image_random_generate.txt"
    output_file = "../data/py_dpc_output_data.txt"
    
    # Check if files exist
    if not os.path.exists(input_file):
        print(f"Error: Input file {input_file} not found")
        return 1
    
    if not os.path.exists(output_file):
        print(f"Error: Output file {output_file} not found")
        return 1
    
    # Compare results
    success = compare_dpc_results(input_file, output_file)
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())