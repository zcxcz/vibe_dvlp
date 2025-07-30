#!/usr/bin/env python3
"""
crop.cmd - 系统性测试crop.py功能的测试脚本
使用方法：python crop.cmd
测试内容包括：
1. 单点坐标测试（6个边界点）
2. 矩形区域测试（5个典型区域）
3. 边界条件测试（无效坐标）
4. 结果验证（像素数量检查）
"""

import os
import json
import subprocess
import sys
from pathlib import Path

# 测试配置
TEST_DIR = Path("/home/sheldon/hls_project/vibe_0726")
VIBE_JSON = TEST_DIR / "data" / "vibe.json"
TEST_TXT = TEST_DIR / "data" / "test.txt"
CROP_OUT = TEST_DIR / "data" / "crop_data_out.txt"

# 测试用例定义 - 基于实际图像尺寸2x4
SINGLE_PIXEL_TESTS = [
    ("左上角", 0, 0, 0, 0),
    ("右上角", 1, 0, 1, 0),
    ("左下角", 0, 3, 0, 3),
    ("右下角", 1, 3, 1, 3),
    ("中间点", 1, 1, 1, 1),
    ("中心偏右", 0, 2, 0, 2)
]

RECTANGLE_TESTS = [
    ("左上角1x1", 0, 0, 0, 0, 1),
    ("右上角1x1", 1, 0, 1, 0, 1),
    ("全图像2x4", 0, 0, 1, 3, 8),
    ("中间2x2", 0, 1, 1, 2, 4),
    ("底部中间1x2", 0, 2, 1, 3, 4)
]

BOUNDARY_TESTS = [
    ("无效X坐标", 2, 0, 2, 0),
    ("无效Y坐标", 0, 4, 0, 4),
    ("起始大于结束", 1, 0, 0, 0)
]

def update_vibe_json(start_x, start_y, end_x, end_y):
    """更新vibe.json中的坐标寄存器值"""
    try:
        with open(VIBE_JSON, 'r') as f:
            config = json.load(f)
        
        config["register_info"]["reg_crop_start_x"]["reg_initial_value"] = [start_x]
        config["register_info"]["reg_crop_start_y"]["reg_initial_value"] = [start_y]
        config["register_info"]["reg_crop_end_x"]["reg_initial_value"] = [end_x]
        config["register_info"]["reg_crop_end_y"]["reg_initial_value"] = [end_y]
        config["register_info"]["reg_crop_enable"]["reg_initial_value"] = [1]
        
        with open(VIBE_JSON, 'w') as f:
            json.dump(config, f, indent=2)
        
        return True
    except Exception as e:
        print(f"更新配置文件失败: {e}")
        return False

def run_crop_test():
    """运行crop测试"""
    try:
        result = subprocess.run([
            "./build/vibe"
        ], cwd=TEST_DIR, capture_output=True, text=True, timeout=10)
        return result.returncode == 0, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        return False, "", "测试超时"
    except Exception as e:
        return False, "", str(e)

def count_pixels_in_file(filepath):
    """计算文件中的像素数量"""
    try:
        with open(filepath, 'r') as f:
            lines = [line.strip() for line in f if line.strip()]
        return len(lines)
    except:
        return 0

def test_single_pixels():
    """测试单点坐标"""
    print("=== 单点坐标测试 ===")
    results = []
    
    for name, x, y, end_x, end_y in SINGLE_PIXEL_TESTS:
        print(f"测试 {name}: ({x},{y})")
        
        if update_vibe_json(x, y, end_x, end_y):
            success, stdout, stderr = run_crop_test()
            
            if success:
                pixel_count = count_pixels_in_file(CROP_OUT)
                if pixel_count == 1:
                    print(f"  ✓ {name}: 成功获取1个像素")
                    results.append(True)
                else:
                    print(f"  ✗ {name}: 期望1个像素，实际{pixel_count}个")
                    results.append(False)
            else:
                print(f"  ✗ {name}: 执行失败 - {stderr}")
                results.append(False)
        else:
            results.append(False)
    
    return results

def test_rectangles():
    """测试矩形区域"""
    print("\n=== 矩形区域测试 ===")
    results = []
    
    for name, x, y, end_x, end_y, expected_pixels in RECTANGLE_TESTS:
        print(f"测试 {name}: ({x},{y})到({end_x},{end_y})")
        
        if update_vibe_json(x, y, end_x, end_y):
            success, stdout, stderr = run_crop_test()
            
            if success:
                pixel_count = count_pixels_in_file(CROP_OUT)
                if pixel_count == expected_pixels:
                    print(f"  ✓ {name}: 成功获取{expected_pixels}个像素")
                    results.append(True)
                else:
                    print(f"  ✗ {name}: 期望{expected_pixels}个像素，实际{pixel_count}个")
                    results.append(False)
            else:
                print(f"  ✗ {name}: 执行失败 - {stderr}")
                results.append(False)
        else:
            results.append(False)
    
    return results

def test_boundaries():
    """测试边界条件"""
    print("\n=== 边界条件测试 ===")
    results = []
    
    for name, x, y, end_x, end_y in BOUNDARY_TESTS:
        print(f"测试 {name}: ({x},{y})到({end_x},{end_y})")
        
        # 直接调用crop.py进行边界测试
        cmd = [
            "python", str(TEST_DIR / "py" / "crop.py"),
            str(TEST_TXT), str(TEST_DIR / "data" / "test_boundary.txt"),
            str(x), str(y), str(end_x), str(end_y), "true",
            "2", "4", "8", "BAYER"
        ]
        
        try:
            result = subprocess.run(cmd, cwd=TEST_DIR, capture_output=True, text=True, timeout=5)
            
            if result.returncode != 0 and "Error" in result.stdout:
                print(f"  ✓ {name}: 正确检测到无效坐标")
                results.append(True)
            else:
                print(f"  ✗ {name}: 应该失败但执行成功")
                results.append(False)
        except Exception as e:
            print(f"  ✗ {name}: 测试异常 - {e}")
            results.append(False)
    
    return results

def main():
    """主测试函数"""
    print("crop.py 系统性测试开始")
    print("=" * 50)
    
    # 检查必要文件
    if not VIBE_JSON.exists():
        print(f"错误: 配置文件 {VIBE_JSON} 不存在")
        sys.exit(1)
    
    # 运行测试
    single_results = test_single_pixels()
    rect_results = test_rectangles()
    boundary_results = test_boundaries()
    
    # 汇总结果
    print("\n" + "=" * 50)
    print("测试结果汇总:")
    print(f"单点坐标测试: {sum(single_results)}/{len(single_results)} 通过")
    print(f"矩形区域测试: {sum(rect_results)}/{len(rect_results)} 通过")
    print(f"边界条件测试: {sum(boundary_results)}/{len(boundary_results)} 通过")
    
    total_tests = len(single_results) + len(rect_results) + len(boundary_results)
    total_passed = sum(single_results) + sum(rect_results) + sum(boundary_results)
    
    print(f"总计: {total_passed}/{total_tests} 通过")
    
    if total_passed == total_tests:
        print("✓ 所有测试通过!")
        sys.exit(0)
    else:
        print("✗ 部分测试失败")
        sys.exit(1)

if __name__ == "__main__":
    main()