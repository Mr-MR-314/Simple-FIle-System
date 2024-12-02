#!/bin/bash

# Define colors for console output
GREEN="\e[32m"
RED="\e[31m"
BLUE="\e[34m"
CYAN="\e[36m"
YELLOW="\e[33m"
RESET="\e[0m"

# Define the executable name
EXECUTABLE="./filesystem" # Change this to the actual compiled program name

# Check if executable exists
if [[ ! -f "$EXECUTABLE" ]]; then
    echo -e "${RED}Error:${RESET} Executable '$EXECUTABLE' not found. Please compile your program first."
    exit 1
fi

# Create test directories for real filesystem
TEST_DIR="test_dir"
COMPRESSED_FILE="test_dir.gz"
DECOMPRESSED_DIR="test_decompressed"

# Clean up old test data
rm -rf $TEST_DIR $COMPRESSED_FILE $DECOMPRESSED_DIR
mkdir $TEST_DIR

# Change to test directory
cd $TEST_DIR

echo -e "${CYAN}Starting Unit and Integration Tests for Filesystem...${RESET}"

# Test 1: Creating files and directories
echo -e "${BLUE}Test 1:${RESET} Creating files and directories..."
echo -e "mkdir folder1\nmkdir folder2\ncd folder1\ntouch file1.txt\ntouch file2.txt\ncd ..\nmkdir folder3" | $EXECUTABLE > /dev/null
if [[ -d "folder1" && -f "folder1/file1.txt" && -f "folder1/file2.txt" && -d "folder2" && -d "folder3" ]]; then
    echo -e "${GREEN}PASS:${RESET} Directories and files created successfully."
else
    echo -e "${RED}FAIL:${RESET} Directory or file creation failed."
fi

# Test 2: Editing files
echo -e "${BLUE}Test 2:${RESET} Editing a file's content..."
echo -e "cd folder1\nedit file1.txt\nThis is a test content.\npwd\ncd .." | $EXECUTABLE > /dev/null
if [[ $(cat folder1/file1.txt) == "This is a test content." ]]; then
    echo -e "${GREEN}PASS:${RESET} File edited successfully."
else
    echo -e "${RED}FAIL:${RESET} File editing failed."
fi

# Test 3: Saving directory structure
echo -e "${BLUE}Test 3:${RESET} Saving directory structure..."
echo -e "save ../$COMPRESSED_FILE\nexit" | $EXECUTABLE > /dev/null
if [[ -f "../$COMPRESSED_FILE" ]]; then
    echo -e "${GREEN}PASS:${RESET} Directory structure saved successfully."
else
    echo -e "${RED}FAIL:${RESET} Directory structure saving failed."
fi

# Test 4: Loading and reconstructing directory structure
cd ..
mkdir $DECOMPRESSED_DIR
cd $DECOMPRESSED_DIR
echo -e "${BLUE}Test 4:${RESET} Loading directory structure..."
echo -e "load ../$COMPRESSED_FILE\npwd\nexit" | $EXECUTABLE > /dev/null
if [[ -d "folder1" && -f "folder1/file1.txt" && -f "folder1/file2.txt" && -d "folder2" && -d "folder3" ]]; then
    echo -e "${GREEN}PASS:${RESET} Directory structure loaded successfully."
else
    echo -e "${RED}FAIL:${RESET} Directory structure loading failed."
fi

# Test 5: Checking for memory leaks using Valgrind
echo -e "${BLUE}Test 5:${RESET} Running Valgrind for memory leak check..."
valgrind --leak-check=full --error-exitcode=1 --log-file=valgrind.log $EXECUTABLE < /dev/null > /dev/null
if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}PASS:${RESET} No memory leaks detected."
else
    echo -e "${RED}FAIL:${RESET} Memory leaks detected. Check valgrind.log for details."
    cat valgrind.log
fi

# Cleanup
cd ..
rm -rf $TEST_DIR $COMPRESSED_FILE $DECOMPRESSED_DIR

echo -e "${CYAN}Tests Completed.${RESET}"
