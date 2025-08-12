# Crop模块回归测试框架

## 🎯 功能概述

这个测试框架提供了自动化的回归测试能力，可以：
- 随机生成各种配置参数进行测试
- 验证不同图像尺寸和裁剪区域的正确性
- 支持指定测试数量和随机种子
- 自动生成测试数据，避免数据不匹配问题

## 🚀 快速开始

### 1. 基本使用
```bash
# 运行50个随机测试用例
./test/run_test.sh 50

# 运行100个测试用例，指定随机种子
./test/run_test.sh 100 123456

# 使用Python直接运行
python3 test/regression_test.py -n 100
```

### 2. 高级用法
```bash
# 指定随机种子进行可重复测试
python3 test/regression_test.py -n 200 -s 42 -o my_results.json

# 查看帮助
python3 test/regression_test.py --help
```

## 📋 测试覆盖范围

### 测试参数范围
- **图像尺寸**: 1×1 到 8×8 像素
- **裁剪模式**: 透传模式(crop_enable=0) 和 裁剪模式(crop_enable=1)
- **裁剪区域**: 任意有效矩形区域
- **边界条件**: 单像素裁剪、全图裁剪、边缘裁剪

### 验证内容
- ✅ 输出像素数量正确性
- ✅ 裁剪区域计算准确性
- ✅ 透传模式数据完整性
- ✅ 边界条件处理
- ✅ 配置参数有效性

## 🔧 命令行参数

```
usage: regression_test.py [-h] [-n COUNT] [-s SEED] [-o OUTPUT]

Crop模块回归测试

options:
  -h, --help            显示帮助信息
  -n COUNT, --count COUNT
                        测试用例数量 (默认: 100)
  -s SEED, --seed SEED  随机种子 (默认: 当前时间)
  -o OUTPUT, --output OUTPUT
                        结果输出文件 (默认: test_results.json)
```

## 📊 输出说明

### 终端输出
```
🚀 开始Crop模块回归测试
📊 测试数量: 50
🎲 随机种子: 1755013373
--------------------------------------------------

🧪 测试 1/50 ✅ 输出正确: 7 pixels
🧪 测试 2/50 ✅ 输出正确: 35 pixels
...

==================================================
📈 测试总结
✅ 通过: 50
❌ 失败: 0
📊 通过率: 100.0%
📋 测试结果已导出到: test_results.json
```

### 结果文件格式
测试完成后会生成`test_results.json`，包含：
- 测试统计信息
- 每个测试的详细配置
- 预期和实际输出
- 失败用例的详细信息

## 🛠️ 故障排除

### 常见问题

1. **"Input data size mismatch"错误**
   - 已修复：框架现在会自动生成匹配尺寸的测试数据

2. **"无法解析输出"错误**
   - 已修复：增强了解析逻辑，支持多种输出格式

3. **权限问题**
   ```bash
   chmod +x test/run_test.sh
   chmod +x test/regression_test.py
   ```

### 调试单个测试

如需调试特定配置，可以手动运行：
```bash
# 手动设置配置
python3 -c "
import json
with open('data/vibe.json', 'r') as f:
    config = json.load(f)
config['register_info']['reg_image_width']['reg_initial_value'] = [3]
config['register_info']['reg_image_height']['reg_initial_value'] = [4]
config['register_info']['reg_crop_enable']['reg_initial_value'] = [1]
config['register_info']['reg_crop_start_x']['reg_initial_value'] = [1]
config['register_info']['reg_crop_start_y']['reg_initial_value'] = [1]
config['register_info']['reg_crop_end_x']['reg_initial_value'] = [2]
config['register_info']['reg_crop_end_y']['reg_initial_value'] = [2]
with open('data/vibe.json', 'w') as f:
    json.dump(config, f, indent=2)
"

# 生成测试数据
echo -e "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12" > data/image.txt

# 运行测试
./build/vibe
```

## 🎯 最佳实践

### 持续集成
```bash
# 在CI/CD中运行
./test/run_test.sh 1000  # 大量测试确保稳定性
```

### 回归验证
```bash
# 验证代码修改后是否引入bug
./test/run_test.sh 50 $(date +%s)  # 使用当前时间作为种子
```

### 边界测试
```bash
# 测试边界条件
python3 test/regression_test.py -n 200 -s 1 -o boundary_test.json
```