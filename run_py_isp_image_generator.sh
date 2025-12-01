#!/bin/bash

# ISP Image Generator Script
# This script generates random image data based on configuration
# Usage: ./run_py_isp_image_generator.sh [--config CONFIG_PATH]

echo "=== ISP Image Generator ==="

# Set working directories
PROJECT_ROOT="/home/sheldon/hls_project/vibe_crop"
PY_DIR="$PROJECT_ROOT/py"
DATA_DIR="$PROJECT_ROOT/data"
CONFIG_DIR="$PROJECT_ROOT/config"

# Default configuration file paths
DEFAULT_CONFIG="$CONFIG_DIR/image_config.json"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --config)
            DEFAULT_CONFIG="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--config CONFIG_PATH]"
            exit 1
            ;;
    esac
done

# Check if necessary directories exist
if [ ! -d "$PY_DIR" ]; then
    echo "[Error]: Python script directory $PY_DIR does not exist"
    exit 1
fi

if [ ! -d "$CONFIG_DIR" ]; then
    echo "[Error]: Configuration directory $CONFIG_DIR does not exist"
    exit 1
fi

# Check required Python script files
if [ ! -f "$PY_DIR/isp_image_generator.py" ]; then
    echo "[Error]: Required Python script $PY_DIR/isp_image_generator.py does not exist"
    exit 1
fi

# Check if configuration files exist
if [ ! -f "$DEFAULT_CONFIG" ]; then
    echo "[Error]: Default configuration file $DEFAULT_CONFIG does not exist"
    exit 1
fi

echo "All dependent files check passed"

# Switch to Python script directory
cd "$PY_DIR"

# Display Python program to be invoked
echo "Python program to be invoked:"
echo "- $PY_DIR/isp_image_generator.py"

# Execute image generation
echo "Executing image generation..."
echo "[Command]: python3 $PY_DIR/isp_image_generator.py --config $DEFAULT_CONFIG"
python3 "$PY_DIR/isp_image_generator.py" --config "$DEFAULT_CONFIG"
GENERATE_EXIT_CODE=$?

if [ $GENERATE_EXIT_CODE -ne 0 ]; then
    echo "[Error]: Image generation failed"
    exit 1
else
    echo "[Success]: Image generation succeeded"
fi

echo "Image generator completed successfully"