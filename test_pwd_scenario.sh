#!/bin/bash

echo "=== Testing PWD behavior after directory deletion ==="

# Create test directory in /tmp to avoid issues
TEST_DIR="/tmp/pwd_test_$$"
mkdir -p "$TEST_DIR"
cd "$TEST_DIR"

echo "1. Created and changed to test directory: $PWD"

# Create folder
mkdir folder
echo "2. Created folder"

# Change to folder
cd folder
echo "3. Changed to folder: $PWD"

# Remove the folder we're in
rm -rf ../folder 2>/dev/null || echo "Could not remove folder (expected if we're in it)"

echo "4. Removed folder, current PWD: $PWD"

# Test bash pwd behavior
echo "5. Bash pwd output:"
pwd 2>&1

# Unset PWD
unset PWD
echo "6. After unsetting PWD, bash pwd output:"
pwd 2>&1

echo ""
echo "=== Now testing minishell behavior ==="

# Change back to test directory and recreate scenario
cd "$TEST_DIR" 2>/dev/null || cd /tmp
mkdir -p folder
cd folder

# Test with minishell
echo "Testing minishell pwd in existing directory:"
echo "pwd" | /mnt/c/Users/morta/OneDrive/Documents/last/minishell

# Remove directory and test
rm -rf ../folder 2>/dev/null

echo "Testing minishell pwd after directory deletion:"
echo "pwd" | /mnt/c/Users/morta/OneDrive/Documents/last/minishell

# Test with PWD unset
unset PWD
echo "Testing minishell pwd with PWD unset:"
echo "pwd" | /mnt/c/Users/morta/OneDrive/Documents/last/minishell

# Cleanup
cd /tmp
rm -rf "$TEST_DIR" 2>/dev/null

echo "=== Test completed ==="
