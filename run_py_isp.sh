#!/bin/bash

# Main ISP Python Processing Script
# This script orchestrates the entire ISP processing pipeline by calling sub-scripts in sequence
# Usage: ./run_py_isp.sh [--config CONFIG_PATH] [--register REGISTER_PATH] [--image IMAGE_PATH]

echo "=== Main ISP Python Processing Script ==="

# Set working directory
PROJECT_ROOT="/home/sheldon/hls_project/vibe_crop"
cd "$PROJECT_ROOT"

# Default configuration file paths
DEFAULT_CONFIG="$PROJECT_ROOT/config/image_config.json"
REGISTER_TABLE="$PROJECT_ROOT/config/register_table.csv"
RANDOM_SRC_IMAGE="$PROJECT_ROOT/data/src_image_random_generate.txt"

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
        --image)
            RANDOM_SRC_IMAGE="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--config CONFIG_PATH] [--register REGISTER_PATH] [--image IMAGE_PATH]"
            exit 1
            ;;
    esac
done

# Define script paths
GENERATOR_SCRIPT="$PROJECT_ROOT/run_py_isp_image_generator.sh"
PROCESSOR_SCRIPT="$PROJECT_ROOT/run_py_isp_image_processor.sh"
CHECKER_SCRIPT="$PROJECT_ROOT/run_py_isp_image_checker.sh"

# Pass arguments to sub-scripts
GENERATOR_ARGS="--config $DEFAULT_CONFIG"
PROCESSOR_ARGS="--register $REGISTER_TABLE --config $DEFAULT_CONFIG $RANDOM_SRC_IMAGE"

# === Step 1: Check for required scripts ===
echo ""
echo "=== Step 1: Checking for required scripts ==="

CHECK_FAILED=0

if [ ! -f "$GENERATOR_SCRIPT" ]; then
    echo "[Error]: Generator script $GENERATOR_SCRIPT does not exist"
    CHECK_FAILED=1
else
    echo "Found: $(basename "$GENERATOR_SCRIPT")"
fi

if [ ! -f "$PROCESSOR_SCRIPT" ]; then
    echo "[Error]: Processor script $PROCESSOR_SCRIPT does not exist"
    CHECK_FAILED=1
else
    echo "Found: $(basename "$PROCESSOR_SCRIPT")"
fi

if [ ! -f "$CHECKER_SCRIPT" ]; then
    echo "[Error]: Checker script $CHECKER_SCRIPT does not exist"
    CHECK_FAILED=1
else
    echo "Found: $(basename "$CHECKER_SCRIPT")"
fi

if [ $CHECK_FAILED -eq 1 ]; then
    echo "[Error]: Missing required scripts, exiting"
    exit 1
fi

echo "All required scripts found."

# === Step 2: Run generator script ===
echo ""
echo "=== Step 2: Running image generator ==="
if ! "$GENERATOR_SCRIPT" $GENERATOR_ARGS; then
    echo "[Error]: Generator script failed"
    exit 1
fi
echo "Generator completed successfully"

# === Step 3: Run processor script ===
echo ""
echo "=== Step 3: Running image processor ==="
if ! "$PROCESSOR_SCRIPT" $PROCESSOR_ARGS; then
    echo "[Error]: Processor script failed"
    exit 1
fi
echo "Processor completed successfully"

# === Step 4: Run checker script ===
echo ""
echo "=== Step 4: Running image checker ==="
if ! "$CHECKER_SCRIPT"; then
    echo "[Error]: Checker script failed"
    exit 1
fi
echo "Checker completed successfully"

# === Final success message ===
echo ""
echo "=== All steps completed successfully ==="
echo "ISP Python processing pipeline finished!"