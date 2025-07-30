#!/bin/bash

# Comprehensive Valgrind Test Script for Minishell GC
# Tests various command combinations to stress-test memory management

echo "🧪 MINISHELL GARBAGE COLLECTOR VALGRIND TEST"
echo "============================================="
echo ""

# Test commands to run
test_commands=(
    "echo hello world"
    "ls -l"
    "pwd"
    "echo test > output.txt"
    "cat output.txt"
    "echo append >> output.txt"
    "ls | grep txt"
    "echo line1 | cat | cat"
    "ls -la | head -5"
    "echo multi > file1.txt && echo test > file2.txt"
    "cat file1.txt file2.txt"
    "rm output.txt file1.txt file2.txt"
    "echo 'quoted string with spaces'"
    'echo "double quotes test"'
    "echo \$HOME"
    "echo \$PATH | head -c 50"
    "ls /tmp | wc -l"
    "echo test | grep test"
)

# Create test input file
echo "Creating test input file..."
{
    for cmd in "${test_commands[@]}"; do
        echo "$cmd"
    done
    echo "exit"
} > minishell_test_input.txt

echo "Test commands prepared:"
for i in "${!test_commands[@]}"; do
    echo "  $((i+1)). ${test_commands[i]}"
done
echo ""

echo "🚀 Running Valgrind with full leak detection..."
echo "Command: valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind_output.log ./minishell < minishell_test_input.txt"
echo ""

# Run valgrind
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind_output.log \
         --suppressions=/dev/null \
         ./minishell < minishell_test_input.txt

echo ""
echo "📊 VALGRIND RESULTS:"
echo "===================="

# Display results
if [ -f valgrind_output.log ]; then
    echo "Full log saved to: valgrind_output.log"
    echo ""
    echo "📈 MEMORY SUMMARY:"
    echo "------------------"
    grep -A 10 "HEAP SUMMARY" valgrind_output.log || echo "No heap summary found"
    
    echo ""
    echo "🔍 LEAK SUMMARY:" 
    echo "----------------"
    grep -A 15 "LEAK SUMMARY" valgrind_output.log || echo "No leak summary found"
    
    echo ""
    echo "❌ ERROR SUMMARY:"
    echo "-----------------"
    grep -A 5 "ERROR SUMMARY" valgrind_output.log || echo "No error summary found"
    
    echo ""
    echo "⚠️  ERRORS DETECTED:"
    echo "-------------------"
    grep -B 2 -A 5 "Invalid\|Conditional\|Uninitialised\|Invalid read\|Invalid write" valgrind_output.log || echo "No memory errors detected!"
    
    echo ""
    echo "💧 LEAKS DETECTED:"
    echo "------------------"
    grep -B 2 -A 10 "definitely lost\|indirectly lost\|possibly lost" valgrind_output.log || echo "No memory leaks detected!"
    
else
    echo "❌ Valgrind output file not found!"
fi

# Cleanup
echo ""
echo "🧹 Cleaning up test files..."
rm -f minishell_test_input.txt
rm -f output.txt file1.txt file2.txt

echo ""
echo "✅ Valgrind test complete!"
echo "📁 Full detailed log: valgrind_output.log"
