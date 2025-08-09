#!/bin/bash

# Valgrind Results Analyzer Script
# Analyzes and summarizes valgrind test results

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
    
    local total_tests=$(grep -c "=== Test" "$file" 2>/dev/null || echo "0")
    local no_leak_tests=$(grep -c "NO LEAKS" "$file" 2>/dev/null || echo "0")
    local leak_tests=$(grep -c "LEAKS DETECTED" "$file" 2>/dev/null || echo "0")
    local fd_leak_tests=$(grep -c "FD LEAKS: YES" "$file" 2>/dev/null || echo "0")
    local fd_no_leak_tests=$(grep -c "FD LEAKS: NO" "$file" 2>/dev/null || echo "0")
    
    echo -e "Total Tests: ${BLUE}$total_tests${NC}"
    echo -e "Memory Leaks: ${GREEN}$no_leak_tests passed${NC} / ${RED}$leak_tests failed${NC}"
    echo -e "FD Leaks: ${GREEN}$fd_no_leak_tests passed${NC} / ${RED}$fd_leak_tests failed${NC}"
    
    if [ $total_tests -gt 0 ]; then
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

# Function to compare multiple test results
compare_results() {
    echo -e "\n${PURPLE}🔄 Comparing All Available Test Results${NC}"
    echo "============================================="
    
    local files=("leak_summary.txt" "comprehensive_leak_summary.txt")
    local names=("Standard Comprehensive" "Enhanced Comprehensive")
    
    for i in "${!files[@]}"; do
        if [ -f "${files[$i]}" ]; then
            analyze_summary "${files[$i]}" "${names[$i]}"
        fi
    done
}

# Function to show recent valgrind logs
show_recent_logs() {
    echo -e "\n${YELLOW}📁 Recent Valgrind Log Files${NC}"
    echo "============================="
    
    local log_files=($(ls -t valgrind_*.log 2>/dev/null | head -5))
    
    if [ ${#log_files[@]} -eq 0 ]; then
        echo -e "${YELLOW}No recent valgrind logs found${NC}"
        return
    fi
    
    for log in "${log_files[@]}"; do
        local size=$(ls -lh "$log" | awk '{print $5}')
        local date=$(ls -l "$log" | awk '{print $6, $7, $8}')
        echo -e "${CYAN}$log${NC} (${size}, ${date})"
    done
    
    echo -e "\n${BLUE}💡 To view a specific log: cat valgrind_N.log${NC}"
}

# Function to clean old logs
clean_old_logs() {
    echo -e "\n${YELLOW}🧹 Cleaning Old Valgrind Logs${NC}"
    echo "=============================="
    
    local log_count=$(ls valgrind_*.log 2>/dev/null | wc -l)
    
    if [ $log_count -eq 0 ]; then
        echo -e "${GREEN}No valgrind logs to clean${NC}"
        return
    fi
    
    echo -e "Found ${BLUE}$log_count${NC} valgrind log files"
    echo -e "${YELLOW}Do you want to delete all valgrind_*.log files? (y/n)${NC}"
    read -r response
    
    if [[ $response =~ ^[Yy]$ ]]; then
        rm -f valgrind_*.log
        echo -e "${GREEN}✅ Cleaned $log_count log files${NC}"
    else
        echo -e "${BLUE}❌ Cleanup cancelled${NC}"
    fi
}

# Function to generate improvement suggestions
suggest_improvements() {
    echo -e "\n${PURPLE}💡 Improvement Suggestions${NC}"
    echo "=========================="
    
    # Check if there are any summary files
    if [ -f "leak_summary.txt" ] || [ -f "comprehensive_leak_summary.txt" ]; then
        # Look for common issues
        local has_memory_leaks=$(grep -l "LEAKS DETECTED" *.txt 2>/dev/null)
        local has_fd_leaks=$(grep -l "FD LEAKS: YES" *.txt 2>/dev/null)
        
        if [ -n "$has_memory_leaks" ]; then
            echo -e "${RED}🔍 Memory Leak Issues Found:${NC}"
            echo "   • Check malloc/free pairs"
            echo "   • Verify proper cleanup in error paths"
            echo "   • Use garbage collector consistently"
            echo "   • Check for double-free errors"
        fi
        
        if [ -n "$has_fd_leaks" ]; then
            echo -e "${RED}🔍 File Descriptor Issues Found:${NC}"
            echo "   • Check pipe() calls have matching close()"
            echo "   • Verify open() calls have matching close()"
            echo "   • Check dup2() usage"
            echo "   • Ensure proper cleanup in child processes"
        fi
        
        if [ -z "$has_memory_leaks" ] && [ -z "$has_fd_leaks" ]; then
            echo -e "${GREEN}🎉 No major issues found! Consider:${NC}"
            echo "   • Running tests with different input sizes"
            echo "   • Testing with more complex command combinations"
            echo "   • Adding stress tests with many pipes"
        fi
    else
        echo -e "${YELLOW}⚠️  No test results found. Run tests first:${NC}"
        echo "   • make valgrind-quick (for quick testing)"
        echo "   • make valgrind-full (for comprehensive testing)"
        echo "   • make valgrind-comprehensive (for extensive testing)"
    fi
}

# Main function
main() {
    echo -e "${GREEN}=== VALGRIND RESULTS ANALYZER ===${NC}"
    echo -e "${BLUE}Analyzing your minishell valgrind test results${NC}"
    
    case "${1:-analyze}" in
        "analyze"|"")
            compare_results
            suggest_improvements
            ;;
        "logs")
            show_recent_logs
            ;;
        "clean")
            clean_old_logs
            ;;
        "help")
            echo -e "\n${YELLOW}Usage: $0 [command]${NC}"
            echo "Commands:"
            echo "  analyze (default) - Analyze test results and provide suggestions"
            echo "  logs             - Show recent valgrind log files"
            echo "  clean            - Clean old valgrind log files"
            echo "  help             - Show this help"
            ;;
        *)
            echo -e "${RED}Unknown command: $1${NC}"
            echo -e "Use ${CYAN}$0 help${NC} for usage information"
            ;;
    esac
    
    echo -e "\n${GREEN}Analysis complete!${NC}"
}

# Run the analyzer
main "$@"
