#!/bin/bash

# Crop模块回归测试脚本
# 使用方法: ./run_test.sh [测试数量] [随机种子]

TEST_COUNT=${1:-50}
SEED=${2:-}

echo "🚀 启动Crop模块回归测试"
echo "📊 测试数量: $TEST_COUNT"
if [ -n "$SEED" ]; then
    echo "🎲 随机种子: $SEED"
fi

# 确保Python脚本可执行
chmod +x /home/sheldon/hls_project/vibe_crop/test/regression_test.py

# 运行测试
cd /home/sheldon/hls_project/vibe_crop
python3 test/regression_test.py -n $TEST_COUNT ${SEED:+-s $SEED}

# 检查测试结果
if [ $? -eq 0 ]; then
    echo "✅ 回归测试完成 - 全部通过!"
else
    echo "❌ 回归测试完成 - 存在失败用例!"
    exit 1
fi