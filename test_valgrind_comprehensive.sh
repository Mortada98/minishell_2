#!/bin/bash

# Enhanced Comprehensive Valgrind Leak Testing Script for Minishell
# Tests various command types with extensive coverage for each category

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Test configuration
VALGRIND_CMD="valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --suppressions=readline.sup --log-file=valgrind_output.log"
MINISHELL="./minishell"
TEST_DIR="valgrind_tests"
LEAK_SUMMARY_FILE="comprehensive_leak_summary.txt"

# Create test directory and files
setup_test_environment() {
    echo -e "${BLUE}Setting up comprehensive test environment...${NC}"
    
    # Create test directory
    mkdir -p "$TEST_DIR"
    cd "$TEST_DIR"
    
    # Create various test files
    echo "Hello World" > test_input.txt
    echo -e "line1\nline2\nline3\nline4\nline5" > multiline.txt
    echo "test content for redirection" > redirect_test.txt
    echo -e "apple\nbanana\ncherry\ndate\nelderberry" > fruits.txt
    echo -e "123\n456\n789\n101112" > numbers.txt
    echo -e "foo bar\nbaz qux\nhello world\ntest case" > words.txt
    echo -e "error line\nwarning message\ninfo log\ndebug trace" > logs.txt
    
    # Create nested directories
    mkdir -p test_subdir/nested
    mkdir -p empty_dir
    echo "content in subdir" > test_subdir/nested_file.txt
    echo "deeply nested content" > test_subdir/nested/deep_file.txt
    
    # Create files with special characters
    echo "file with spaces content" > "file with spaces.txt"
    echo "special chars: !@#$%^&*()" > special_chars.txt
    
    # Create executable test script
    echo -e "#!/bin/bash\necho 'Hello from script'\necho 'Second line'" > test_script.sh
    chmod +x test_script.sh
    
    # Create heredoc test content
    cat > heredoc_content.txt << 'EOL'
This is a test
for heredoc
functionality
with multiple lines
and various content
EOL
    
    cd ..
    echo -e "${GREEN}Comprehensive test environment setup complete${NC}"
}

# Function to run a test case
run_test_case() {
    local test_name="$1"
    local commands="$2"
    local test_num="$3"
    
    echo -e "\n${YELLOW}[$test_num] Testing: $test_name${NC}"
    echo "Commands: $commands"
    
    # Create input file for this test
    echo -e "$commands\nexit" > "test_input_$test_num.txt"
    
    # Run valgrind with minishell
    echo -e "${BLUE}Running valgrind...${NC}"
    timeout 30s $VALGRIND_CMD $MINISHELL < "test_input_$test_num.txt" > "test_output_$test_num.txt" 2>&1
    
    # Check valgrind output
    if [ -f "valgrind_output.log" ]; then
        # Extract leak summary
        echo "=== Test $test_num: $test_name ===" >> "$LEAK_SUMMARY_FILE"
        echo "Commands: $commands" >> "$LEAK_SUMMARY_FILE"
        
        # Check for leaks
        if grep -q "definitely lost: 0 bytes" valgrind_output.log && 
           grep -q "indirectly lost: 0 bytes" valgrind_output.log && 
           grep -q "possibly lost: 0 bytes" valgrind_output.log; then
            echo -e "${GREEN}✓ No leaks detected${NC}"
            echo "Result: NO LEAKS" >> "$LEAK_SUMMARY_FILE"
        else
            echo -e "${RED}✗ Memory leaks detected${NC}"
            echo "Result: LEAKS DETECTED" >> "$LEAK_SUMMARY_FILE"
            grep "definitely lost\|indirectly lost\|possibly lost" valgrind_output.log >> "$LEAK_SUMMARY_FILE"
        fi
        
        # Check for file descriptor leaks
        if grep -q "Open file descriptor" valgrind_output.log; then
            echo -e "${RED}✗ File descriptor leaks detected${NC}"
            echo "FD LEAKS: YES" >> "$LEAK_SUMMARY_FILE"
            grep "Open file descriptor" valgrind_output.log >> "$LEAK_SUMMARY_FILE"
        else
            echo -e "${GREEN}✓ No file descriptor leaks${NC}"
            echo "FD LEAKS: NO" >> "$LEAK_SUMMARY_FILE"
        fi
        
        echo "" >> "$LEAK_SUMMARY_FILE"
        
        # Archive the valgrind output
        mv valgrind_output.log "valgrind_$test_num.log"
    else
        echo -e "${RED}✗ Valgrind output not found${NC}"
    fi
    
    # Clean up input file
    rm -f "test_input_$test_num.txt"
}

# Main testing function
run_all_tests() {
    echo -e "${BLUE}Starting comprehensive valgrind leak tests with enhanced coverage...${NC}"
    
    # Initialize summary file
    echo "COMPREHENSIVE VALGRIND LEAK TEST SUMMARY" > "$LEAK_SUMMARY_FILE"
    echo "=========================================" >> "$LEAK_SUMMARY_FILE"
    echo "Date: $(date)" >> "$LEAK_SUMMARY_FILE"
    echo "" >> "$LEAK_SUMMARY_FILE"
    
    local test_count=1
    
    # 1. Enhanced Built-in Commands Tests
    echo -e "\n${PURPLE}=== ENHANCED BUILT-IN COMMANDS TESTS ===${NC}"
    
    # Echo variations
    run_test_case "Echo simple" "echo Hello World" $((test_count++))
    run_test_case "Echo with -n flag" "echo -n Hello" $((test_count++))
    run_test_case "Echo with quotes" "echo \"Hello World\"" $((test_count++))
    run_test_case "Echo with single quotes" "echo 'Hello World'" $((test_count++))
    run_test_case "Echo multiple arguments" "echo arg1 arg2 arg3 arg4" $((test_count++))
    run_test_case "Echo with special chars" "echo '!@#\$%^&*()'" $((test_count++))
    run_test_case "Echo long string" "echo 'This is a very long string that contains many words and should test memory allocation properly'" $((test_count++))
    
    # PWD variations
    run_test_case "PWD simple" "pwd" $((test_count++))
    run_test_case "PWD after cd" "cd $TEST_DIR\npwd" $((test_count++))
    run_test_case "PWD multiple calls" "pwd\npwd\npwd" $((test_count++))
    
    # Environment variables
    run_test_case "ENV simple" "env" $((test_count++))
    run_test_case "ENV with grep" "env | grep HOME" $((test_count++))
    run_test_case "ENV sorted" "env | sort" $((test_count++))
    
    # Export variations
    run_test_case "Export simple" "export TEST_VAR=hello" $((test_count++))
    run_test_case "Export and echo" "export TEST_VAR=hello\necho \$TEST_VAR" $((test_count++))
    run_test_case "Export multiple" "export VAR1=val1\nexport VAR2=val2\necho \$VAR1 \$VAR2" $((test_count++))
    run_test_case "Export with spaces" "export SPACE_VAR='hello world'" $((test_count++))
    run_test_case "Export overwrite" "export TEST=first\nexport TEST=second\necho \$TEST" $((test_count++))
    
    # Unset variations
    run_test_case "Unset simple" "export TEST_VAR=hello\nunset TEST_VAR\necho \$TEST_VAR" $((test_count++))
    run_test_case "Unset multiple" "export VAR1=val1\nexport VAR2=val2\nunset VAR1 VAR2" $((test_count++))
    
    # CD variations
    run_test_case "CD to directory" "cd $TEST_DIR\npwd" $((test_count++))
    run_test_case "CD back to parent" "cd $TEST_DIR\ncd ..\npwd" $((test_count++))
    run_test_case "CD relative path" "cd $TEST_DIR/test_subdir\npwd" $((test_count++))
    
    # 2. External commands
    echo -e "\n${BLUE}=== EXTERNAL COMMANDS TESTS ===${NC}"
    
    run_test_case "LS command" "ls" $((test_count++))
    run_test_case "LS with flags" "ls -la" $((test_count++))
    run_test_case "Cat command" "cat $TEST_DIR/test_input.txt" $((test_count++))
    run_test_case "Grep command" "grep Hello $TEST_DIR/test_input.txt" $((test_count++))
    run_test_case "WC command" "wc -l $TEST_DIR/multiline.txt" $((test_count++))
    
    # 3. Simple pipes
    echo -e "\n${BLUE}=== SIMPLE PIPE TESTS ===${NC}"
    
    run_test_case "Echo pipe grep" "echo Hello World | grep Hello" $((test_count++))
    run_test_case "Cat pipe grep" "cat $TEST_DIR/multiline.txt | grep line1" $((test_count++))
    run_test_case "LS pipe wc" "ls | wc -l" $((test_count++))
    run_test_case "Echo pipe cat" "echo test | cat" $((test_count++))
    run_test_case "Env pipe grep" "env | grep PATH" $((test_count++))
    
    # 4. Multiple pipes
    echo -e "\n${BLUE}=== MULTIPLE PIPE TESTS ===${NC}"
    
    run_test_case "Three command pipe" "echo Hello World Test | grep World | wc -w" $((test_count++))
    run_test_case "Cat pipe grep pipe wc" "cat $TEST_DIR/multiline.txt | grep line | wc -l" $((test_count++))
    run_test_case "LS pipe grep pipe sort" "ls -la | grep ^d | sort" $((test_count++))
    run_test_case "Echo pipe cat pipe grep" "echo testing pipes | cat | grep pipe" $((test_count++))
    run_test_case "Four command pipe" "echo one two three four | tr ' ' '\n' | sort | head -2" $((test_count++))
    
    # 5. Output redirections
    echo -e "\n${BLUE}=== OUTPUT REDIRECTION TESTS ===${NC}"
    
    run_test_case "Echo redirect" "echo Hello > $TEST_DIR/output1.txt" $((test_count++))
    run_test_case "LS redirect" "ls > $TEST_DIR/output2.txt" $((test_count++))
    run_test_case "Echo append" "echo World >> $TEST_DIR/output1.txt" $((test_count++))
    run_test_case "PWD redirect" "pwd > $TEST_DIR/pwd_output.txt" $((test_count++))
    run_test_case "Env redirect" "env > $TEST_DIR/env_output.txt" $((test_count++))
    
    # 6. Input redirections
    echo -e "\n${BLUE}=== INPUT REDIRECTION TESTS ===${NC}"
    
    run_test_case "Cat input redirect" "cat < $TEST_DIR/test_input.txt" $((test_count++))
    run_test_case "Grep input redirect" "grep Hello < $TEST_DIR/test_input.txt" $((test_count++))
    run_test_case "WC input redirect" "wc -l < $TEST_DIR/multiline.txt" $((test_count++))
    
    # 7. Heredoc tests
    echo -e "\n${BLUE}=== HEREDOC TESTS ===${NC}"
    
    run_test_case "Simple heredoc" "cat << EOF\nHello\nWorld\nEOF" $((test_count++))
    run_test_case "Heredoc with grep" "grep Hello << EOF\nHello World\nTest line\nEOF" $((test_count++))
    run_test_case "Heredoc with wc" "wc -l << EOF\nline1\nline2\nline3\nEOF" $((test_count++))
    
    # 8. Complex combinations
    echo -e "\n${BLUE}=== COMPLEX COMBINATION TESTS ===${NC}"
    
    run_test_case "Pipe with output redirect" "echo Hello World | grep Hello > $TEST_DIR/combo1.txt" $((test_count++))
    run_test_case "Input redirect with pipe" "cat < $TEST_DIR/test_input.txt | grep Hello" $((test_count++))
    run_test_case "Multiple pipes with redirect" "echo one two three | tr ' ' '\n' | sort > $TEST_DIR/sorted.txt" $((test_count++))
    run_test_case "Heredoc with pipe" "cat << EOF | grep test\ntest line\nother line\nEOF" $((test_count++))
    run_test_case "Complex pipe chain" "ls -la | grep ^- | wc -l > $TEST_DIR/file_count.txt" $((test_count++))
    
    # 9. Built-in commands with redirections
    echo -e "\n${BLUE}=== BUILT-INS WITH REDIRECTIONS ===${NC}"
    
    run_test_case "Echo with redirect" "echo Built-in test > $TEST_DIR/builtin_out.txt" $((test_count++))
    run_test_case "PWD with redirect" "pwd > $TEST_DIR/pwd_builtin.txt" $((test_count++))
    run_test_case "Env with pipe" "env | grep HOME" $((test_count++))
    run_test_case "Export with redirect" "export | grep PATH > $TEST_DIR/export_out.txt" $((test_count++))
    
    # 10. Built-in commands with pipes
    echo -e "\n${BLUE}=== BUILT-INS WITH PIPES ===${NC}"
    
    run_test_case "Echo pipe to external" "echo Hello | cat" $((test_count++))
    run_test_case "PWD pipe to grep" "pwd | grep home" $((test_count++))
    run_test_case "Env pipe to sort" "env | sort | head -5" $((test_count++))
    
    # 11. Error cases and edge cases
    echo -e "\n${BLUE}=== ERROR AND EDGE CASES ===${NC}"
    
    run_test_case "Non-existent command" "nonexistentcommand" $((test_count++))
    run_test_case "Invalid redirection" "echo test > /dev/null/invalid" $((test_count++))
    run_test_case "Empty command" "" $((test_count++))
    run_test_case "Multiple spaces" "echo    hello    world" $((test_count++))
    run_test_case "Special characters" "echo 'hello world' | grep hello" $((test_count++))
    
    # 12. Variable expansion tests
    echo -e "\n${BLUE}=== VARIABLE EXPANSION TESTS ===${NC}"
    
    run_test_case "Basic variable expansion" "export VAR=test\necho \$VAR" $((test_count++))
    run_test_case "Variable in command" "export CMD=echo\n\$CMD hello" $((test_count++))
    run_test_case "Exit status variable" "echo test\necho \$?" $((test_count++))
    run_test_case "Undefined variable" "echo \$UNDEFINED_VAR" $((test_count++))
    
    echo -e "\n${GREEN}All tests completed!${NC}"
    echo -e "${BLUE}Check $LEAK_SUMMARY_FILE for detailed results${NC}"
}

# Function to generate final report
generate_report() {
    echo -e "\n${BLUE}Generating final report...${NC}"
    
    echo "" >> "$LEAK_SUMMARY_FILE"
    echo "FINAL SUMMARY" >> "$LEAK_SUMMARY_FILE"
    echo "=============" >> "$LEAK_SUMMARY_FILE"
    
    local total_tests=$(grep -c "=== Test" "$LEAK_SUMMARY_FILE")
    local no_leak_tests=$(grep -c "NO LEAKS" "$LEAK_SUMMARY_FILE")
    local leak_tests=$(grep -c "LEAKS DETECTED" "$LEAK_SUMMARY_FILE")
    local fd_leak_tests=$(grep -c "FD LEAKS: YES" "$LEAK_SUMMARY_FILE")
    
    echo "Total tests run: $total_tests" >> "$LEAK_SUMMARY_FILE"
    echo "Tests with no leaks: $no_leak_tests" >> "$LEAK_SUMMARY_FILE"
    echo "Tests with memory leaks: $leak_tests" >> "$LEAK_SUMMARY_FILE"
    echo "Tests with FD leaks: $fd_leak_tests" >> "$LEAK_SUMMARY_FILE"
    
    echo -e "\n${GREEN}=== FINAL REPORT ===${NC}"
    echo -e "Total tests run: ${BLUE}$total_tests${NC}"
    echo -e "Tests with no leaks: ${GREEN}$no_leak_tests${NC}"
    echo -e "Tests with memory leaks: ${RED}$leak_tests${NC}"
    echo -e "Tests with FD leaks: ${RED}$fd_leak_tests${NC}"
    
    if [ $leak_tests -eq 0 ] && [ $fd_leak_tests -eq 0 ]; then
        echo -e "\n${GREEN}🎉 Congratulations! No memory or file descriptor leaks detected!${NC}"
    else
        echo -e "\n${RED}⚠️  Memory or file descriptor leaks detected. Check individual test logs for details.${NC}"
    fi
}

# Cleanup function
cleanup() {
    echo -e "\n${BLUE}Cleaning up test files...${NC}"
    rm -f test_input_*.txt test_output_*.txt
    rm -rf "$TEST_DIR"
    echo -e "${GREEN}Cleanup complete${NC}"
}

# Main execution
main() {
    echo -e "${GREEN}=== MINISHELL VALGRIND LEAK TESTING SCRIPT ===${NC}"
    echo -e "${BLUE}This script will test various command types for memory leaks${NC}"
    echo -e "${YELLOW}Make sure minishell is compiled and readline.sup exists${NC}"
    
    # Check if minishell exists
    if [ ! -f "$MINISHELL" ]; then
        echo -e "${RED}Error: minishell executable not found. Please compile first.${NC}"
        exit 1
    fi
    
    # Check if readline.sup exists
    if [ ! -f "readline.sup" ]; then
        echo -e "${YELLOW}Warning: readline.sup not found. Readline leaks may appear in results.${NC}"
    fi
    
    # Check if valgrind is installed
    if ! command -v valgrind &> /dev/null; then
        echo -e "${RED}Error: valgrind is not installed. Please install valgrind first.${NC}"
        exit 1
    fi
    
    setup_test_environment
    run_all_tests
    generate_report
    
    # Ask user if they want to cleanup
    echo -e "\n${YELLOW}Do you want to clean up test files? (y/n)${NC}"
    read -r cleanup_choice
    if [[ $cleanup_choice =~ ^[Yy]$ ]]; then
        cleanup
    else
        echo -e "${BLUE}Test files and logs preserved for manual inspection${NC}"
    fi
    
    echo -e "\n${GREEN}Script completed successfully!${NC}"
}

# Run the script
main "$@"
