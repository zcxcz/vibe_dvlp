#!/usr/bin/env python3
import json
import csv

# 读取vibe.json文件
with open('/home/sheldon/hls_project/vibe_crop/src/vibe.json', 'r') as json_file:
    data = json.load(json_file)

# 获取register_info部分
register_info = data.get('register_info', {})

# 创建register_table.csv文件并写入数据
with open('/home/sheldon/hls_project/vibe_crop/src/register_table.csv', 'w', newline='') as csv_file:
    csv_writer = csv.writer(csv_file)
    # 写入表头，按照用户要求包含reg_name, bitwidth, initial_value, cons_min, cons_max
    csv_writer.writerow(['reg_name', 'bitwidth', 'initial_value', 'cons_min', 'cons_max'])
    
    # 遍历所有寄存器信息
    for reg_name, reg_data in register_info.items():
        # 获取各个字段的值
        bit_width = reg_data.get('reg_bit_width', '')
        
        # 处理初始值，如果有多个值用空格分隔
        initial_values = reg_data.get('reg_initial_value', [])
        # 将列表转换为字符串，用空格分隔
        initial_value_str = ' '.join(map(str, initial_values))
        
        value_min = reg_data.get('reg_value_min', '')
        value_max = reg_data.get('reg_value_max', '')
        
        # 写入CSV行，包含寄存器完整名称
        csv_writer.writerow([reg_name, bit_width, initial_value_str, value_min, value_max])
        
print("转换完成！register_table.csv已按照新格式生成。")