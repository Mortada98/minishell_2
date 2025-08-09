#!/bin/bash

# Test Level Comparison Script

echo "=== MINISHELL VALGRIND TEST LEVELS COMPARISON ==="
echo

echo "🔹 QUICK TEST (quick_valgrind_test.sh)"
echo "   • ~35 essential test cases"
echo "   • Fast execution (~2-3 minutes)"
echo "   • Perfect for development iterations"
echo "   • Covers basic functionality"
echo

echo "🔸 COMPREHENSIVE TEST (test_valgrind_leaks.sh)"
echo "   • ~56 thorough test cases"
echo "   • Medium execution (~5-7 minutes)"
echo "   • Good for regular testing"
echo "   • Covers all major features"
echo

echo "🔶 ENHANCED COMPREHENSIVE TEST (test_valgrind_comprehensive.sh)"
echo "   • 200+ extensive test cases"
echo "   • Longer execution (~15-20 minutes)"
echo "   • Best for final validation"
echo "   • Covers edge cases and advanced scenarios"
echo

echo "📊 TEST CATEGORIES BREAKDOWN:"
echo

echo "Category                    | Quick | Comprehensive | Enhanced"
echo "---------------------------|-------|---------------|----------"
echo "Built-in Commands          |   4   |       9       |    25"
echo "External Commands          |   4   |       5       |    20"
echo "Simple Pipes              |   3   |       5       |    15"
echo "Multiple Pipes            |   2   |       5       |    10"
echo "Output Redirections       |   2   |       5       |    15"
echo "Input Redirections        |   1   |       3       |    10"
echo "Heredoc                   |   2   |       3       |    12"
echo "Complex Combinations      |   2   |       5       |    12"
echo "Built-ins + Redirections  |   0   |       4       |     8"
echo "Built-ins + Pipes         |   0   |       3       |    15"
echo "Error Cases               |   2   |       7       |    15"
echo "Variable Expansion        |   3   |       4       |    15"
echo "Signal Handling           |   0   |       0       |     2"
echo "Quote Handling            |   0   |       0       |     7"
echo "Wildcard/Globbing         |   0   |       0       |     4"
echo "---------------------------|-------|---------------|----------"
echo "TOTAL                     |  ~35  |      ~56      |   200+"

echo
echo "🎯 RECOMMENDED USAGE:"
echo "• During development: make valgrind-quick"
echo "• Before commits: make valgrind-full"
echo "• Before submission: make valgrind-comprehensive"
echo
echo "💡 TIP: Start with quick tests and move to comprehensive as your code stabilizes"
