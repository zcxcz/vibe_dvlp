#!/bin/bash

# ISP Image Processor (DPC) Script
# This script processes image data using DPC algorithm
# Usage: ./run_py_isp_image_processor.sh [--register REGISTER_PATH] [--config CONFIG_PATH] [IMAGE_PATH]

echo "=== ISP Image Processor (DPC) ==="

# Set working directories
PROJECT_ROOT="/home/sheldon/hls_project/vibe_crop"
PY_DIR="$PROJECT_ROOT/py"
DATA_DIR="$PROJECT_ROOT/data"
CONFIG_DIR="$PROJECT_ROOT/config"

# Default configuration file paths
DEFAULT_CONFIG="$CONFIG_DIR/image_config.json"
REGISTER_TABLE="$CONFIG_DIR/register_table.csv"
RANDOM_SRC_IMAGE="$DATA_DIR/src_image_random_generate.txt"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --config)
            DEFAULT_CONFIG="$2"
            shift 2
            ;;
        --register)
            REGISTER_TABLE="$2"
            shift 2
            ;;
        -*)
            echo "Unknown option: $1"
            echo "Usage: $0 [--register REGISTER_PATH] [--config CONFIG_PATH] [IMAGE_PATH]"
            exit 1
            ;;
        *)
            # Positional argument for image path
            RANDOM_SRC_IMAGE="$1"
            shift
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

if [ ! -d "$DATA_DIR" ]; then
    echo "[Warning]: Data directory $DATA_DIR does not exist, will be created automatically"
    mkdir -p "$DATA_DIR"
fi

# Check required Python script files
if [ ! -f "$PY_DIR/isp_dpc_processor.py" ]; then
    echo "[Error]: Required Python script $PY_DIR/isp_dpc_processor.py does not exist"
    exit 1
fi

# Check if configuration files exist
if [ ! -f "$DEFAULT_CONFIG" ]; then
    echo "[Error]: Default configuration file $DEFAULT_CONFIG does not exist"
    exit 1
fi

if [ ! -f "$REGISTER_TABLE" ]; then
    echo "[Error]: Register table file $REGISTER_TABLE does not exist"
    exit 1
fi

echo "All dependent files check passed"

# Switch to Python script directory
cd "$PY_DIR"

# Display Python program to be invoked
echo "Python program to be invoked:"
echo "- $PY_DIR/isp_dpc_processor.py"

# Execute DPC processing
echo "Executing DPC processing..."
echo "[Command]: python3 $PY_DIR/isp_dpc_processor.py --register $REGISTER_TABLE --config $DEFAULT_CONFIG $RANDOM_SRC_IMAGE"
python3 "$PY_DIR/isp_dpc_processor.py" --register "$REGISTER_TABLE" --config "$DEFAULT_CONFIG" "$RANDOM_SRC_IMAGE"
DPC_EXIT_CODE=$?

if [ $DPC_EXIT_CODE -ne 0 ]; then
    echo "[Error]: DPC processing failed"
    exit 1
else
    echo "[Success]: DPC processing succeeded"
fi

echo "Image processor completed successfully"