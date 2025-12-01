#!/usr/bin/env python3
"""
Python ISP DPC (Defective Pixel Correction) Processing Script

This script implements DPC processing similar to alg_dpc.cpp
It loads register configuration from register_table.csv,
reads RAW image data, applies DPC algorithm, and outputs processed data.
"""

import csv
import json
import os
import sys
from typing import List, Dict, Tuple


class ISPDPCProcessor:
    """DPC (Defective Pixel Correction) Processor"""
    
    def __init__(self):
        self.registers = {}
        self.image_width = 0
        self.image_height = 0
        self.dpc_enable = True
        self.dpc_threshold = 30
        self.output_path = ""
        
    def load_registers_from_csv(self, csv_path: str) -> Dict[str, int]:
        """Load register configuration from CSV file"""
        print(f"Loading registers from: {csv_path}")
        
        registers = {}
        try:
            with open(csv_path, 'r') as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    reg_name = row['reg_name']
                    initial_value = int(row['initial_value'])
                    registers[reg_name] = initial_value
                    
                    # Store key registers as attributes
                    if reg_name == 'reg_image_width':
                        self.image_width = initial_value
                    elif reg_name == 'reg_image_height':
                        self.image_height = initial_value
                    elif reg_name == 'reg_dpc_enable':
                        self.dpc_enable = bool(initial_value)
                    elif reg_name == 'reg_dpc_threshold':
                        self.dpc_threshold = initial_value
                        
        except FileNotFoundError:
            print(f"Error: Register CSV file not found: {csv_path}")
            sys.exit(1)
        except Exception as e:
            print(f"Error loading registers: {e}")
            sys.exit(1)
            
        self.registers = registers
        print(f"Loaded {len(registers)} registers")
        print(f"Image size: {self.image_width}x{self.image_height}")
        print(f"DPC enable: {self.dpc_enable}, threshold: {self.dpc_threshold}")
        
        return registers
    
    def load_image_config(self, json_path: str) -> Dict:
        """Load image configuration from JSON file"""
        print(f"Loading image config from: {json_path}")
        
        try:
            with open(json_path, 'r') as jsonfile:
                config = json.load(jsonfile)
                
            # Get output path for DPC result
            json_output_path = config.get('image_pattern', {}).get('py_dpc_output_path', 
                                                                  'data/py_dpc_output_data.txt')
            # Convert to absolute path from project root
            self.output_path = os.path.join('..', json_output_path)
            print(f"Output path from JSON: {json_output_path}")
            print(f"Final output path: {self.output_path}")
            return config
            
        except FileNotFoundError:
            print(f"Error: Image config JSON file not found: {json_path}")
            sys.exit(1)
        except json.JSONDecodeError as e:
            print(f"Error parsing JSON file: {e}")
            sys.exit(1)
    
    def load_raw_image_data(self, image_path: str) -> List[int]:
        """Load RAW image data from text file"""
        print(f"Loading RAW image data from: {image_path}")
        
        image_data = []
        try:
            with open(image_path, 'r') as f:
                for line_num, line in enumerate(f, 1):
                    line = line.strip()
                    if not line or line.startswith('#'):
                        continue
                        
                    # Parse format: "00ff  # (   0,    0)"
                    parts = line.split('#')
                    if len(parts) < 1:
                        continue
                        
                    hex_value = parts[0].strip()
                    try:
                        pixel_value = int(hex_value, 16)
                        image_data.append(pixel_value)
                    except ValueError:
                        print(f"Warning: Invalid hex value '{hex_value}' at line {line_num}")
                        continue
                        
        except FileNotFoundError:
            print(f"Error: Image data file not found: {image_path}")
            sys.exit(1)
            
        expected_size = self.image_width * self.image_height
        actual_size = len(image_data)
        
        if actual_size != expected_size:
            print(f"Warning: Image data size mismatch. Expected: {expected_size}, Got: {actual_size}")
            # For small test images, adjust dimensions to match actual data
            if actual_size < expected_size:
                # Calculate appropriate dimensions for the actual data size
                test_width = int(actual_size ** 0.5)  # Square approximation
                test_height = actual_size // test_width
                if actual_size % test_width != 0:
                    test_height += 1
                
                print(f"Adjusting to test dimensions: {test_width}x{test_height}")
                self.image_width = test_width
                self.image_height = test_height
            
        print(f"Loaded {len(image_data)} pixels")
        return image_data
    
    def get_pixel_with_mirror_boundary(self, image_data: List[int], x: int, y: int) -> int:
        """Get pixel value with mirror boundary handling"""
        # Apply mirror boundary conditions
        x = max(0, min(self.image_width - 1, x))
        y = max(0, min(self.image_height - 1, y))
        return image_data[y * self.image_width + x]
    
    def apply_dpc_processing(self, input_image: List[int]) -> List[int]:
        """Apply DPC (Defective Pixel Correction) algorithm"""
        if not self.dpc_enable:
            print("DPC processing disabled")
            return input_image.copy()
            
        print("Applying DPC processing...")
        output_image = input_image.copy()
        
        # Process each pixel
        for y in range(self.image_height):
            for x in range(self.image_width):
                p0 = input_image[y * self.image_width + x]
                
                # --- Bad Pixel Detection ---
                # Condition 1: Check if center pixel is outside min/max range of 5x5 window
                min_neighbor = 65535
                max_neighbor = 0
                
                # 5x5 window specific positions (cross pattern)
                window_positions = [
                    (-2, -2), (-2, 0), (-2, 2),
                    (0, -2),          (0, 2),
                    (2, -2),  (2, 0), (2, 2)
                ]
                
                for dx, dy in window_positions:
                    neighbor = self.get_pixel_with_mirror_boundary(input_image, x + dx, y + dy)
                    min_neighbor = min(min_neighbor, neighbor)
                    max_neighbor = max(max_neighbor, neighbor)
                
                cond1_met = (p0 < min_neighbor) or (p0 > max_neighbor)
                
                # Condition 2: Check if differences with all 8 neighbors exceed threshold
                cond2_met = True
                if cond1_met:
                    # 3x3 neighborhood
                    neighbor_positions = [
                        (-1, -1), (-1, 0), (-1, 1),
                        (0, -1),          (0, 1),
                        (1, -1),  (1, 0), (1, 1)
                    ]
                    
                    for dx, dy in neighbor_positions:
                        neighbor = self.get_pixel_with_mirror_boundary(input_image, x + dx, y + dy)
                        if abs(p0 - neighbor) <= self.dpc_threshold:
                            cond2_met = False
                            break
                else:
                    cond2_met = False
                
                # If both conditions met, it's a bad pixel
                if cond1_met and cond2_met:
                    # --- Bad Pixel Correction ---
                    # Calculate gradients in 4 directions
                    
                    # Vertical gradient
                    p_up = self.get_pixel_with_mirror_boundary(input_image, x, y - 2)
                    p_down = self.get_pixel_with_mirror_boundary(input_image, x, y + 2)
                    dv = abs(-p_up + 2 * p0 - p_down)
                    
                    # Horizontal gradient
                    p_left = self.get_pixel_with_mirror_boundary(input_image, x - 2, y)
                    p_right = self.get_pixel_with_mirror_boundary(input_image, x + 2, y)
                    dh = abs(-p_left + 2 * p0 - p_right)
                    
                    # Left diagonal gradient (upper-left to lower-right)
                    p_ul = self.get_pixel_with_mirror_boundary(input_image, x - 2, y - 2)
                    p_dr = self.get_pixel_with_mirror_boundary(input_image, x + 2, y + 2)
                    ddl = abs(-p_ul + 2 * p0 - p_dr)
                    
                    # Right diagonal gradient (upper-right to lower-left)
                    p_ur = self.get_pixel_with_mirror_boundary(input_image, x + 2, y - 2)
                    p_dl = self.get_pixel_with_mirror_boundary(input_image, x - 2, y + 2)
                    ddr = abs(-p_ur + 2 * p0 - p_dl)
                    
                    # Find minimum gradient direction
                    gradients = [dv, dh, ddl, ddr]
                    min_grad_idx = gradients.index(min(gradients))
                    
                    # Interpolate along minimum gradient direction
                    if min_grad_idx == 0:  # Vertical
                        new_p0 = (self.get_pixel_with_mirror_boundary(input_image, x, y - 1) + 
                                 self.get_pixel_with_mirror_boundary(input_image, x, y + 1)) // 2
                    elif min_grad_idx == 1:  # Horizontal
                        new_p0 = (self.get_pixel_with_mirror_boundary(input_image, x - 1, y) + 
                                 self.get_pixel_with_mirror_boundary(input_image, x + 1, y)) // 2
                    elif min_grad_idx == 2:  # Left diagonal
                        new_p0 = (self.get_pixel_with_mirror_boundary(input_image, x - 1, y - 1) + 
                                 self.get_pixel_with_mirror_boundary(input_image, x + 1, y + 1)) // 2
                    else:  # Right diagonal
                        new_p0 = (self.get_pixel_with_mirror_boundary(input_image, x + 1, y - 1) + 
                                 self.get_pixel_with_mirror_boundary(input_image, x - 1, y + 1)) // 2
                    
                    # Update output with corrected value
                    output_image[y * self.image_width + x] = new_p0
        
        print("DPC processing completed")
        return output_image
    
    def save_output_data(self, output_image: List[int], output_path: str = None):
        """Save processed image data to output file"""
        if output_path is None:
            output_path = self.output_path
            
        print(f"Saving output data to: {output_path}")
        
        # Create output directory if it doesn't exist
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        
        try:
            with open(output_path, 'w') as f:
                pixel_idx = 0
                for y in range(self.image_height):
                    for x in range(self.image_width):
                        pixel_value = output_image[pixel_idx]
                        # Format: "00ff  # (   0,    0)"
                        f.write(f"{pixel_value:04x}  # ({x:4d}, {y:4d})\n")
                        pixel_idx += 1
                        
            print(f"Successfully saved {len(output_image)} pixels to {output_path}")
            
        except Exception as e:
            print(f"Error saving output data: {e}")
            sys.exit(1)
    
    def process(self, register_table_csv_path: str, src_image_config_json_path: str, random_src_image_path: str):
        """Main processing pipeline"""
        print("=== Python ISP DPC Processing ===")
        
        # Step 1: Load register configuration
        self.load_registers_from_csv(register_table_csv_path)
        
        # Step 2: Load image configuration
        self.load_image_config(src_image_config_json_path)
        
        # Step 3: Load RAW image data
        input_image = self.load_raw_image_data(random_src_image_path)
        
        # Step 4: Apply DPC processing
        output_image = self.apply_dpc_processing(input_image)
        
        # Step 5: Save output data
        self.save_output_data(output_image)
        
        print("=== Processing completed successfully ===")


def main():
    """Main function"""
    # Default file paths - adjusted for py directory location
    register_table_csv_path = "../src/register_table.csv"
    src_image_config_json_path = "../src/image_config.json"
    random_src_image_path = "../data/src_image_random_generate.txt"
    
    # Parse command line arguments
    import argparse
    parser = argparse.ArgumentParser(description='ISP DPC Processor')
    parser.add_argument('--register', help='Path to register table CSV file')
    parser.add_argument('--config', help='Path to image config JSON file')
    parser.add_argument('register_pos', nargs='?', help='Register table CSV file path (positional)')
    parser.add_argument('config_pos', nargs='?', help='Image config JSON file path (positional)')
    parser.add_argument('image_pos', nargs='?', help='Input image data file path (positional)')
    
    args = parser.parse_args()
    
    # Use --register argument if provided, otherwise use positional argument
    if args.register:
        register_table_csv_path = args.register
    elif args.register_pos:
        register_table_csv_path = args.register_pos
    
    # Use --config argument if provided, otherwise use positional argument
    if args.config:
        src_image_config_json_path = args.config
    elif args.config_pos:
        src_image_config_json_path = args.config_pos
        
    # Use positional argument for image file if provided
    if args.image_pos:
        random_src_image_path = args.image_pos
    
    # Check if files exist
    if not os.path.exists(register_table_csv_path):
        print(f"Error: Register CSV file not found: {register_table_csv_path}")
        sys.exit(1)
        
    if not os.path.exists(src_image_config_json_path):
        print(f"Error: Image config JSON file not found: {src_image_config_json_path}")
        sys.exit(1)
        
    if not os.path.exists(random_src_image_path):
        print(f"Error: RAW image data file not found: {random_src_image_path}")
        sys.exit(1)
    
    # Create processor and run
    processor = ISPDPCProcessor()
    processor.process(register_table_csv_path, src_image_config_json_path, random_src_image_path)


if __name__ == "__main__":
    main()