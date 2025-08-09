#!/bin/bash

# Simple Valgrind Results Analyzer Script

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# Function to analyze a summary file
analyze_summary() {
    local file="$1"
    local test_type="$2"
    
    if [ ! -f "$file" ]; then
        echo -e "${RED}❌ Summary file $file not found${NC}"
        return 1
    fi
    
    echo -e "\n${BLUE}📊 Analysis of $test_type Results${NC}"
    echo -e "${CYAN}File: $file${NC}"
    echo "========================================"
    
    # Count results more carefully
    local total_tests=0
    local no_leak_tests=0
    local leak_tests=0
    local fd_leak_tests=0
    local fd_no_leak_tests=0
    
    if [ -f "$file" ]; then
        total_tests=$(grep -c "=== Test" "$file" 2>/dev/null)
        no_leak_tests=$(grep -c "Result: NO LEAKS" "$file" 2>/dev/null)
        leak_tests=$(grep -c "Result: LEAKS DETECTED" "$file" 2>/dev/null)
        fd_leak_tests=$(grep -c "FD LEAKS: YES" "$file" 2>/dev/null)
        fd_no_leak_tests=$(grep -c "FD LEAKS: NO" "$file" 2>/dev/null)
    fi
    
    # Set defaults if empty
    total_tests=${total_tests:-0}
    no_leak_tests=${no_leak_tests:-0}
    leak_tests=${leak_tests:-0}
    fd_leak_tests=${fd_leak_tests:-0}
    fd_no_leak_tests=${fd_no_leak_tests:-0}
    
    echo -e "Total Tests: ${BLUE}$total_tests${NC}"
    echo -e "Memory Leaks: ${GREEN}$no_leak_tests passed${NC} / ${RED}$leak_tests failed${NC}"
    echo -e "FD Leaks: ${GREEN}$fd_no_leak_tests passed${NC} / ${RED}$fd_leak_tests failed${NC}"
    
    if [ "$total_tests" -gt 0 ]; then
        local memory_success_rate=$((no_leak_tests * 100 / total_tests))
        local fd_success_rate=$((fd_no_leak_tests * 100 / total_tests))
        
        echo -e "Memory Success Rate: ${CYAN}${memory_success_rate}%${NC}"
        echo -e "FD Success Rate: ${CYAN}${fd_success_rate}%${NC}"
        
        # Overall grade
        if [ "$leak_tests" -eq 0 ] && [ "$fd_leak_tests" -eq 0 ]; then
            echo -e "Overall Grade: ${GREEN}🏆 PERFECT${NC}"
        elif [ "$leak_tests" -eq 0 ]; then
            echo -e "Overall Grade: ${YELLOW}⭐ GOOD (Memory: Perfect, FD: Issues)${NC}"
        elif [ "$fd_leak_tests" -eq 0 ]; then
            echo -e "Overall Grade: ${YELLOW}⭐ GOOD (Memory: Issues, FD: Perfect)${NC}"
        else
            echo -e "Overall Grade: ${RED}⚠️  NEEDS WORK${NC}"
        fi
    fi
}

# Main function
main() {
    echo -e "${GREEN}=== VALGRIND RESULTS ANALYZER ===${NC}"
    echo -e "${BLUE}Analyzing your minishell valgrind test results${NC}"
    
    # Analyze available summary files
    if [ -f "leak_summary.txt" ]; then
        analyze_summary "leak_summary.txt" "Standard Comprehensive"
    fi
    
    if [ -f "comprehensive_leak_summary.txt" ]; then
        analyze_summary "comprehensive_leak_summary.txt" "Enhanced Comprehensive"
    fi
    
    # Show suggestions
    echo -e "\n${PURPLE}💡 Quick Analysis${NC}"
    echo "================="
    
    if [ -f "leak_summary.txt" ]; then
        local has_memory_leaks=$(grep -q "LEAKS DETECTED" leak_summary.txt && echo "yes" || echo "no")
        local has_fd_leaks=$(grep -q "FD LEAKS: YES" leak_summary.txt && echo "yes" || echo "no")
        
        if [ "$has_memory_leaks" = "no" ] && [ "$has_fd_leaks" = "no" ]; then
            echo -e "${GREEN}🎉 Excellent! No issues found${NC}"
        elif [ "$has_memory_leaks" = "no" ]; then
            echo -e "${YELLOW}⚠️  FD leaks detected - check pipe/file handling${NC}"
        elif [ "$has_fd_leaks" = "no" ]; then
            echo -e "${YELLOW}⚠️  Memory leaks detected - check malloc/free${NC}"
        else
            echo -e "${RED}⚠️  Both memory and FD leaks detected${NC}"
        fi
    else
        echo -e "${YELLOW}📋 No test results found. Run: make valgrind-quick${NC}"
    fi
    
    echo -e "\n${GREEN}Analysis complete!${NC}"
}

# Run the analyzer
main "$@"
