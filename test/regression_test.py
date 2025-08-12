#!/usr/bin/env python3
"""
Crop模块回归测试框架
支持随机配置测试和指定条数的回归测试
"""

import json
import os
import sys
import subprocess
import random
import time
from pathlib import Path

class CropRegressionTester:
    def __init__(self, test_count=100, seed=None):
        """
        初始化测试器
        
        Args:
            test_count: 测试用例数量
            seed: 随机种子，为None时使用当前时间
        """
        self.test_count = test_count
        self.seed = seed or int(time.time())
        random.seed(self.seed)
        
        # 路径配置
        self.project_root = Path("/home/sheldon/hls_project/vibe_crop")
        self.config_file = self.project_root / "data" / "vibe.json"
        self.vibe_executable = self.project_root / "build" / "vibe"
        
        # 测试统计
        self.passed = 0
        self.failed = 0
        self.test_results = []
        
    def generate_random_config(self):
        """生成随机配置"""
        image_width = random.randint(1, 8)  # 限制小尺寸便于测试
        image_height = random.randint(1, 8)
        
        # 裁剪参数
        crop_enable = random.choice([0, 1])
        
        if crop_enable == 1:
            # 确保裁剪区域有效
            max_start_x = max(0, image_width - 1)
            max_start_y = max(0, image_height - 1)
            
            crop_start_x = random.randint(0, max_start_x)
            crop_start_y = random.randint(0, max_start_y)
            
            # 确保end >= start，且不超过图像边界
            crop_end_x = random.randint(crop_start_x, image_width - 1)
            crop_end_y = random.randint(crop_start_y, image_height - 1)
        else:
            # 透传模式下裁剪参数设为0
            crop_start_x = 0
            crop_start_y = 0
            crop_end_x = 0
            crop_end_y = 0
            
        return {
            "image_width": image_width,
            "image_height": image_height,
            "crop_enable": crop_enable,
            "crop_start_x": crop_start_x,
            "crop_start_y": crop_start_y,
            "crop_end_x": crop_end_x,
            "crop_end_y": crop_end_y
        }
    
    def load_config(self):
        """加载当前配置"""
        with open(self.config_file, 'r') as f:
            return json.load(f)
    
    def save_config(self, config):
        """保存配置到文件"""
        base_config = self.load_config()
        
        # 更新相关寄存器值
        base_config["register_info"]["reg_image_width"]["reg_initial_value"] = [config["image_width"]]
        base_config["register_info"]["reg_image_height"]["reg_initial_value"] = [config["image_height"]]
        base_config["register_info"]["reg_crop_start_x"]["reg_initial_value"] = [config["crop_start_x"]]
        base_config["register_info"]["reg_crop_start_y"]["reg_initial_value"] = [config["crop_start_y"]]
        base_config["register_info"]["reg_crop_end_x"]["reg_initial_value"] = [config["crop_end_x"]]
        base_config["register_info"]["reg_crop_end_y"]["reg_initial_value"] = [config["crop_end_y"]]
        base_config["register_info"]["reg_crop_enable"]["reg_initial_value"] = [config["crop_enable"]]
        
        with open(self.config_file, 'w') as f:
            json.dump(base_config, f, indent=2)
    
    def calculate_expected_output(self, config):
        """计算预期输出像素数量"""
        if config["crop_enable"] == 0:
            # 透传模式：完整图像
            return config["image_width"] * config["image_height"]
        else:
            # 裁剪模式：裁剪区域
            width = config["crop_end_x"] - config["crop_start_x"] + 1
            height = config["crop_end_y"] - config["crop_start_y"] + 1
            return max(0, width) * max(0, height)
    
    def generate_test_data(self, width, height):
        """生成随机测试数据"""
        test_data = []
        for _ in range(width * height):
            test_data.append(str(random.randint(0, 1023)))  # 10位数据范围
        return test_data
    
    def create_test_image(self, width, height):
        """创建测试图像文件"""
        test_data = self.generate_test_data(width, height)
        image_file = self.project_root / "data" / "image.txt"
        
        with open(image_file, 'w') as f:
            f.write('\n'.join(test_data))
        
        return len(test_data)
    
    def run_single_test(self, test_config):
        """运行单个测试"""
        try:
            # 创建测试数据
            expected_pixels = self.calculate_expected_output(test_config)
            actual_data_size = self.create_test_image(
                test_config["image_width"], 
                test_config["image_height"]
            )
            
            # 保存配置
            self.save_config(test_config)
            
            # 运行测试
            result = subprocess.run(
                [str(self.vibe_executable)],
                cwd=str(self.project_root),
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode != 0:
                return False, f"程序运行失败: {result.stderr}"
            
            # 解析输出
            output_lines = result.stdout.split('\n')
            for line in output_lines:
                line = line.strip()
                if "HLS model completed" in line and "pixels" in line:
                    try:
                        # 尝试从"Output: X pixels"格式中提取数字
                        parts = line.split()
                        for i, part in enumerate(parts):
                            if part == "Output:" and i+1 < len(parts):
                                actual_output = int(parts[i+1])
                                
                                if actual_output == expected_pixels:
                                    return True, f"输出正确: {actual_output} pixels"
                                else:
                                    return False, f"输出错误: 预期{expected_pixels}, 实际{actual_output}"
                    except (ValueError, IndexError):
                        continue
            
            # 如果没有找到正确的格式，检查是否有错误信息
            if "ERROR:" in result.stdout:
                for line in output_lines:
                    if "ERROR:" in line:
                        return False, f"程序错误: {line.strip()}"
            
            return False, f"无法解析输出: {result.stdout}"
            
        except Exception as e:
            return False, str(e)
    
    def run_regression_test(self):
        """运行回归测试"""
        print(f"🚀 开始Crop模块回归测试")
        print(f"📊 测试数量: {self.test_count}")
        print(f"🎲 随机种子: {self.seed}")
        print("-" * 50)
        
        # 保存原始配置
        original_config = self.load_config()
        
        try:
            for i in range(self.test_count):
                print(f"\n🧪 测试 {i+1}/{self.test_count}", end="")
                
                # 生成随机配置
                test_config = self.generate_random_config()
                
                # 运行测试
                success, message = self.run_single_test(test_config)
                
                if success:
                    self.passed += 1
                    print(f" ✅ {message}")
                else:
                    self.failed += 1
                    print(f" ❌ {message}")
                    print(f"   配置: {test_config}")
                
                # 记录详细结果
                self.test_results.append({
                    'test_id': i+1,
                    'config': test_config,
                    'expected': self.calculate_expected_output(test_config),
                    'success': success,
                    'message': message
                })
        
        finally:
            # 恢复原始配置
            with open(self.config_file, 'w') as f:
                json.dump(original_config, f, indent=2)
        
        # 打印总结
        print("\n" + "=" * 50)
        print("📈 测试总结")
        print(f"✅ 通过: {self.passed}")
        print(f"❌ 失败: {self.failed}")
        print(f"📊 通过率: {self.passed/self.test_count*100:.1f}%")
        
        if self.failed > 0:
            print("\n❗ 失败用例详情:")
            for result in self.test_results:
                if not result['success']:
                    print(f"  测试{result['test_id']}: {result['message']}")
                    print(f"    配置: {result['config']}")
        
        return self.failed == 0
    
    def export_results(self, filename="test_results.json"):
        """导出测试结果"""
        results = {
            'test_count': self.test_count,
            'seed': self.seed,
            'passed': self.passed,
            'failed': self.failed,
            'results': self.test_results
        }
        
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"📋 测试结果已导出到: {filename}")

def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(description='Crop模块回归测试')
    parser.add_argument('-n', '--count', type=int, default=100, 
                       help='测试用例数量 (默认: 100)')
    parser.add_argument('-s', '--seed', type=int, default=None,
                       help='随机种子 (默认: 当前时间)')
    parser.add_argument('-o', '--output', type=str, default='test_results.json',
                       help='结果输出文件 (默认: test_results.json)')
    
    args = parser.parse_args()
    
    # 检查可执行文件
    if not os.path.exists('/home/sheldon/hls_project/vibe_crop/build/vibe'):
        print("❌ 错误: 请先编译项目 (运行: cmake --build build)")
        sys.exit(1)
    
    # 运行测试
    tester = CropRegressionTester(test_count=args.count, seed=args.seed)
    success = tester.run_regression_test()
    
    if success:
        tester.export_results(args.output)
        print("🎉 所有测试通过!")
        sys.exit(0)
    else:
        tester.export_results(args.output)
        print("💥 测试失败!")
        sys.exit(1)

if __name__ == "__main__":
    main()