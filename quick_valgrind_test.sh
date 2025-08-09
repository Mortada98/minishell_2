#!/bin/bash

# Quick Valgrind Test Script for Minishell
# A simplified version for quick leak testing

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

VALGRIND_CMD="valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --suppressions=readline.sup"
MINISHELL="./minishell"

# Enhanced Quick test cases
QUICK_TESTS=(
    # Basic built-ins
    "echo hello"
    "echo -n test"
    "pwd"
    "env | head -5"
    
    # Variable tests
    "export TEST=value; echo \$TEST"
    "export VAR1=hello; export VAR2=world; echo \$VAR1 \$VAR2"
    "unset HOME; echo \$HOME"
    
    # CD tests
    "cd /tmp; pwd; cd -"
    "cd ~; pwd"
    
    # Simple pipes
    "echo hello world | grep hello"
    "ls | grep test"
    "env | grep PATH"
    "echo 'one two three' | wc -w"
    
    # Multiple pipes
    "echo 'c\na\nb' | sort | head -2"
    "ls -la | grep ^d | wc -l"
    "env | sort | head -3"
    
    # Redirections
    "echo test > output.tmp"
    "echo append >> output.tmp"
    "cat < /etc/passwd | head -3"
    "ls > list.tmp; cat list.tmp"
    
    # Heredoc
    "cat << EOF\nHello\nWorld\nEOF"
    "grep test << EOF\ntest line\nother line\nEOF"
    
    # Combinations
    "echo hello | grep hello > result.tmp"
    "cat << EOF | sort\nzebra\napple\nEOF"
    
    # Error cases
    "nonexistentcommand"
    "echo test > /dev/null/invalid"
    
    # External commands
    "ls -la"
    "cat /etc/hostname"
    "wc -l /etc/passwd"
    "grep root /etc/passwd"
    
    # Complex cases
    "echo 'line1\nline2\nline3' | grep line | wc -l"
    "export CMD=echo; \$CMD 'variable command'"
)

echo -e "${GREEN}=== QUICK VALGRIND LEAK TEST ===${NC}"

for i in "${!QUICK_TESTS[@]}"; do
    test_cmd="${QUICK_TESTS[$i]}"
    echo -e "\n${YELLOW}Test $((i+1)): $test_cmd${NC}"
    
    echo -e "$test_cmd\nexit" | timeout 10s $VALGRIND_CMD $MINISHELL > /dev/null 2> valgrind_quick.log
    
    if grep -q "definitely lost: 0 bytes" valgrind_quick.log && 
       grep -q "indirectly lost: 0 bytes" valgrind_quick.log; then
        echo -e "${GREEN}✓ No leaks${NC}"
    else
        echo -e "${RED}✗ Leaks detected${NC}"
        grep "definitely lost\|indirectly lost" valgrind_quick.log
    fi
    
    rm -f valgrind_quick.log output.tmp
done

echo -e "\n${BLUE}Quick test completed. Run ./test_valgrind_leaks.sh for comprehensive testing.${NC}"
