# ISP源图像生成器

## 功能概述

`py_isp_src_image_generator.py` 是一个灵活的ISP源图像生成工具，支持根据配置文件生成不同格式的随机图像数据。

## 主要特性

### 支持的图像格式
- **BAYER**: 拜耳格式RAW数据 (单通道)
- **RGB**: RGB三通道数据
- **YUV**: YUV三通道数据 (支持YUV422, YUV420等变体)

### 支持的位宽
- **8位**: 数据范围 0-255 (十六进制: 00-ff)
- **16位**: 数据范围 0-65535 (十六进制: 0000-ffff)
- 更高位宽: 自动适配到16位范围

### 配置控制
- **generate_random_src_image_enable**: 控制是否生成随机图像
- **src_image_bitwidth**: 设置数据位宽
- **src_image_format**: 设置图像格式
- **random_src_image_path**: 设置输出文件路径

## 使用方法

### 基本用法
```bash
# 使用默认配置文件 (../src/image_config.json)
python3 py_isp_src_image_generator.py

# 指定配置文件
python3 py_isp_src_image_generator.py ../src/image_config_test.json

# 指定配置和图像尺寸
python3 py_isp_src_image_generator.py ../src/image_config_test.json 64 64
```

### 配置文件格式
配置文件需要包含以下结构：
```json
{
  "image_format": {
    "src_image_format": "BAYER",     // 图像格式: BAYER, RGB, YUV
    "src_image_bitwidth": 8,         // 位宽: 8, 16等
    "generate_random_src_image_enable": 1  // 1=启用, 0=禁用
  },
  "image_pattern": {
    "random_src_image_path": "data/output.txt"  // 输出路径
  }
}
```

## 输出格式

生成的文件包含以下信息：
```
# ISP Source Image Data
# Format: BAYER
# Bitwidth: 8
# Generated: 2025-10-15 23:47:10
# Total pixels: 1024
#
85  # (   0,    0)
18  # (   1,    0)
cd  # (   2,    0)
...
```

### 不同格式的数据示例

**BAYER格式 (8位)**:
```
85  # (   0,    0)
18  # (   1,    0)
```

**RGB格式 (8位)**:
```
22a4b5  # (   0,    0)  # R=0x22, G=0xa4, B=0xb5
86fa95  # (   1,    0)  # R=0x86, G=0xfa, B=0x95
```

**YUV格式 (8位)**:
```
f18e65  # (   0,    0)  # Y=0xf1, U=0x8e, V=0x65
a157f7  # (   1,    0)  # Y=0xa1, U=0x57, V=0xf7
```

**BAYER格式 (16位)**:
```
58a1  # (   0,    0)
01ea  # (   1,    0)
```

## 测试配置

项目中提供了多个测试配置文件：

1. **默认配置**: `../src/image_config.json` (BAYER, 8位)
2. **RGB测试**: `../src/image_config_test_rgb.json` (RGB, 8位)
3. **YUV测试**: `../src/image_config_test_yuv.json` (YUV, 8位)
4. **16位测试**: `../src/image_config_test_16bit.json` (BAYER, 16位)
5. **禁用生成**: `../src/image_config_test_disabled.json` (生成禁用)

## 集成使用

生成的图像文件可以直接用于ISP处理流程：
```bash
# 1. 生成源图像
python3 py_isp_src_image_generator.py

# 2. 运行DPC处理
python3 py_isp_dpc.py

# 3. 比较结果
python3 compare_dpc_results.py
```

## 注意事项

1. **文件路径**: 脚本会自动调整相对路径，确保在`py/`目录下运行
2. **目录创建**: 输出目录不存在时会自动创建
3. **格式验证**: 不支持的格式会自动回退到BAYER格式
4. **位宽限制**: 超过16位的位宽会被限制到16位范围

## 扩展功能

该生成器设计灵活，易于扩展支持：
- 更多图像格式 (如CMYK, HSV等)
- 自定义像素排列模式
- 噪声模型添加
- 图像尺寸自适应
- 批量生成模式