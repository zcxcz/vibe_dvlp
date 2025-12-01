#!/bin/csh

# ISP Image Generator and DPC Processor Invocation Script
# This script processes in three steps:
# 1. Detect all dependent files and directories
# 2. Algorithm execution phase
# 3. Check all output files

echo "=== ISP Image Generator and DPC Processor Automation Script ==="

# Set working directories
setenv PROJECT_ROOT /home/sheldon/hls_project/vibe_crop
setenv PY_DIR $PROJECT_ROOT/py
setenv DATA_DIR $PROJECT_ROOT/data
setenv CONFIG_DIR $PROJECT_ROOT/src

# === Step 1: Detect all dependent files and directories ===
echo ""
echo "=== Step 1: Detect all dependent files and directories ==="

# Initialize check flags
setenv CHECK_FAILED 0

# Check if necessary directories exist
if (! -d $PY_DIR) then
    echo "[Step1-Error1]: Python script directory $PY_DIR does not exist"
    setenv CHECK_FAILED 1
endif

if (! -d $DATA_DIR) then
    echo "[Step1-Warning1]: Data directory $DATA_DIR does not exist, will be created automatically"
    mkdir -p $DATA_DIR
endif

if (! -d $CONFIG_DIR) then
    echo "[Step1-Error2]: Configuration directory $CONFIG_DIR does not exist"
    setenv CHECK_FAILED 1
endif

# Check required Python script files
setenv REQUIRED_SCRIPTS "isp_image_generator.py isp_dpc_processor.py"
setenv SCRIPT_INDEX 1
foreach script ($REQUIRED_SCRIPTS)
    if (! -f $PY_DIR/$script) then
        echo "[Step1-Error3.$SCRIPT_INDEX]: Required Python script $PY_DIR/$script does not exist"
        setenv CHECK_FAILED 1
    endif
    @ SCRIPT_INDEX++
end

# Default configuration file paths
setenv DEFAULT_CONFIG $CONFIG_DIR/image_config.json
setenv REGISTER_TABLE $CONFIG_DIR/register_table.csv

# Check if configuration files exist
if (! -f $DEFAULT_CONFIG) then
    echo "[Step1-Error4]: Default configuration file $DEFAULT_CONFIG does not exist"
    setenv CHECK_FAILED 1
endif

if (! -f $REGISTER_TABLE) then
    echo "[Step1-Error5]: Register table file $REGISTER_TABLE does not exist"
    setenv CHECK_FAILED 1
endif

# Print check results collectively
echo ""
echo "--- Dependency files and directories check results ---"
echo "Project root directory: $PROJECT_ROOT"
echo "Python script directory: $PY_DIR"
echo "Data directory: $DATA_DIR"
echo "Configuration directory: $CONFIG_DIR"
echo "Default configuration file: $DEFAULT_CONFIG"
echo "Register table file: $REGISTER_TABLE"

# Exit with error if any dependencies are missing
if ($CHECK_FAILED == 1) then
    echo "[Step1-Error6]: Missing dependent files or directories, please check the above report"
    exit 1
endif

echo "All dependent files and directories check passed"

# Switch to Python script directory
cd $PY_DIR

# === Step 2: Algorithm execution phase ===
echo ""
echo "=== Step 2: Algorithm execution phase ==="

# Display Python programs to be invoked
echo "Python programs to be invoked:"
echo "- $PY_DIR/isp_image_generator.py"
echo "- $PY_DIR/isp_dpc_processor.py"

# Execute image generation (with numbered status prompts)
echo "Executing image generation..."
echo "[Step2-Command1]: python3 $PY_DIR/isp_image_generator.py --config $DEFAULT_CONFIG"
python3 $PY_DIR/isp_image_generator.py --config $DEFAULT_CONFIG
setenv GENERATE_EXIT_CODE $?
if ($GENERATE_EXIT_CODE != 0) then
    echo "[Step2-Error1]: Image generation failed"
    setenv STEP2_FAILED 1
else
    echo "[Step2-Success1]: Image generation succeeded"
endif

# Execute DPC processing (with numbered status prompts)
echo ""
echo "Executing DPC processing..."
echo "[Step2-Command2]: python3 $PY_DIR/isp_dpc_processor.py --register $REGISTER_TABLE --config $DEFAULT_CONFIG ../data/src_image_random_generate.txt"
python3 $PY_DIR/isp_dpc_processor.py --register $REGISTER_TABLE --config $DEFAULT_CONFIG ../data/src_image_random_generate.txt
setenv DPC_EXIT_CODE $?
if ($DPC_EXIT_CODE != 0) then
    echo "[Step2-Error2]: DPC processing failed"
    setenv STEP2_FAILED 1
else
    echo "[Step2-Success2]: DPC processing succeeded"
endif

echo "Algorithm execution phase completed"

# === Step 3: Check all output files ===
echo ""
echo "=== Step 3: Check all output files ==="

# Define the list of output files to check
setenv OUTPUT_FILES "src_image_random_generate.txt py_dpc_output_data.txt"

# Initialize check counter and failure flag
setenv FILE_CHECK_INDEX 1
setenv MISSING_FILES ""
setenv CHECK_OUTPUT_FAILED 0

# Switch to data directory for checking
cd $DATA_DIR

echo "Start checking output files..."
foreach file ($OUTPUT_FILES)
    if (! -f $file) then
        echo "[Step3-Error$FILE_CHECK_INDEX]: Output file $file does not exist"
        setenv MISSING_FILES "$MISSING_FILES $file"
        setenv CHECK_OUTPUT_FAILED 1
    else
        echo "[Step3-Success$FILE_CHECK_INDEX]: Output file $file exists"
        # Display file size and first/last few lines as verification
        setenv FILE_SIZE `wc -c $file | awk '{print $1}'`
        setenv FILE_LINES `wc -l $file | awk '{print $1}'`
        echo "  File size: ${FILE_SIZE} bytes"
        echo "  Total lines: ${FILE_LINES} lines"
        echo "  File content preview:"
        if ($FILE_LINES > 10) then
            echo "  --- First 5 lines ---"
            head -n 5 $file
            echo "  --- Last 5 lines ---"
            tail -n 5 $file
        else
            cat $file
        endif
        echo ""
    endif
    @ FILE_CHECK_INDEX++
end

# Final check result determination
echo "Output file check completed"
if ($CHECK_OUTPUT_FAILED == 1) then
    echo "[Step3-FinalError]: Missing output files: $MISSING_FILES"
    exit 1
else
    echo "[Step3-FinalSuccess]: All output files check passed"
endif

echo ""
echo "=== 所有步骤完成 ==="
echo "脚本执行成功！"