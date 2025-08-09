# Valgrind Leak Testing Scripts for Minishell

This directory contains comprehensive scripts for testing memory leaks in your minishell implementation using Valgrind.

## Files

### Main Testing Scripts

1. **`quick_valgrind_test.sh`** - Quick leak testing script
   - ~35 essential test cases
   - Fast execution (2-3 minutes)
   - Perfect for development iterations

2. **`test_valgrind_leaks.sh`** - Comprehensive leak testing script
   - ~56 thorough test cases
   - Medium execution (5-7 minutes)
   - Good for regular testing cycles

3. **`test_valgrind_comprehensive.sh`** - Enhanced comprehensive testing script
   - 200+ extensive test cases covering all edge cases
   - Longer execution (15-20 minutes)
   - Best for final validation and submission

4. **`valgrind_test_config.conf`** - Configuration file
   - Customize test settings and options
   - Enable/disable specific test categories

5. **`test_comparison.sh`** - Show differences between test levels
   - Compare test coverage across different scripts
   - Usage recommendations

### Prerequisites

1. **Valgrind**: Install valgrind if not already installed
   ```bash
   # Ubuntu/Debian
   sudo apt-get install valgrind
   
   # macOS (using Homebrew)
   brew install valgrind
   ```

2. **Minishell**: Compile your minishell first
   ```bash
   make
   ```

3. **Readline Suppression File**: Ensure `readline.sup` exists to suppress readline library leaks

## Usage

### Quick Testing
```bash
# Fast development testing (~35 tests)
./quick_valgrind_test.sh

# Or using Makefile
make valgrind-quick
```

### Comprehensive Testing
```bash
# Thorough testing (~56 tests)
./test_valgrind_leaks.sh

# Or using Makefile
make valgrind-full
```

### Enhanced Comprehensive Testing
```bash
# Extensive testing (200+ tests)
./test_valgrind_comprehensive.sh

# Or using Makefile
make valgrind-comprehensive
```

### Compare Test Levels
```bash
# See differences between test scripts
./test_comparison.sh
```

## Test Categories

The comprehensive script tests the following categories:

### 1. Built-in Commands
- `echo` (with and without flags)
- `pwd`
- `env`
- `export` / `unset`
- `cd`

### 2. External Commands
- `ls`, `cat`, `grep`, `wc`
- Various command-line tools

### 3. Simple Pipes
- Two-command pipelines
- Built-in to external command pipes
- External to external command pipes

### 4. Multiple Pipes
- Three or more command pipelines
- Complex data processing chains

### 5. Redirections
- Output redirection (`>`, `>>`)
- Input redirection (`<`)
- Combined with other features

### 6. Heredoc
- Simple heredoc
- Heredoc with pipes
- Multi-line heredoc

### 7. Complex Combinations
- Pipes with redirections
- Multiple features combined
- Real-world usage scenarios

### 8. Error Cases
- Invalid commands
- Invalid paths
- Edge cases and error handling

### 9. Variable Expansion
- Environment variables
- Exit status (`$?`)
- Undefined variables

## Output

### Leak Summary Report
The script generates `leak_summary.txt` containing:
- Test results for each scenario
- Memory leak details
- File descriptor leak information
- Final summary statistics

### Individual Test Logs
Each test generates a separate valgrind log file (`valgrind_N.log`) for detailed analysis.

### Console Output
Real-time feedback with color-coded results:
- 🟢 Green: No leaks detected
- 🔴 Red: Leaks found
- 🟡 Yellow: Warnings or information

## Interpreting Results

### Memory Leaks
- **Definitely lost**: Memory that was allocated but never freed
- **Indirectly lost**: Memory lost due to losing pointers to it
- **Possibly lost**: Memory that might be lost (uncertain)
- **Still reachable**: Memory still accessible but not freed

### File Descriptor Leaks
- Open file descriptors that weren't closed
- Can cause resource exhaustion

### Clean Results
A clean run should show:
```
definitely lost: 0 bytes in 0 blocks
indirectly lost: 0 bytes in 0 blocks
possibly lost: 0 bytes in 0 blocks
```

## Common Issues

### Readline Suppressions
If you see readline-related leaks, ensure `readline.sup` is present and contains:
```
{
   ignore_libcrypto_conditional_jump_errors
   Memcheck:Leak
   ...
   obj:*/libreadline.so.*
}
```

### False Positives
Some libraries may show "still reachable" memory, which is often acceptable for cleanup at program exit.

### Timeouts
If tests timeout, increase the timeout values in the script or configuration file.

## Customization

### Adding Custom Tests
Edit the script to add your own test cases:
```bash
run_test_case "My custom test" "my_command args" $((test_count++))
```

### Modifying Valgrind Options
Change the `VALGRIND_CMD` variable to adjust valgrind parameters:
```bash
VALGRIND_CMD="valgrind --leak-check=full --show-leak-kinds=definite"
```

### Configuration File
Use `valgrind_test_config.conf` to enable/disable test categories and customize settings.

## Troubleshooting

### Script Permissions
Make scripts executable:
```bash
chmod +x test_valgrind_leaks.sh
chmod +x quick_valgrind_test.sh
```

### Missing Dependencies
Ensure all required tools are installed:
- valgrind
- timeout command
- Standard Unix utilities (grep, cat, etc.)

### Minishell Not Found
The script looks for `./minishell` by default. Ensure:
1. Minishell is compiled (`make`)
2. You're running from the correct directory
3. The executable name matches

## Best Practices

1. **Run tests regularly** during development
2. **Fix leaks incrementally** rather than accumulating them
3. **Test edge cases** that might not be covered
4. **Use quick tests** for rapid iteration
5. **Run comprehensive tests** before submission

## Example Output

```
=== VALGRIND LEAK TEST SUMMARY ===
Date: Mon Aug  9 10:30:00 2025

[1] Testing: Echo command
Commands: echo Hello World
✓ No leaks detected
✓ No file descriptor leaks

[2] Testing: Simple pipe
Commands: echo test | grep test
✓ No leaks detected
✓ No file descriptor leaks

=== FINAL REPORT ===
Total tests run: 52
Tests with no leaks: 52
Tests with memory leaks: 0
Tests with FD leaks: 0

🎉 Congratulations! No memory or file descriptor leaks detected!
```

## Contributing

Feel free to add more test cases or improve the scripts. Common additions:
- More complex command combinations
- Additional built-in commands
- Edge cases specific to your implementation
- Performance benchmarks

---

**Note**: These scripts are designed for educational purposes and minishell projects. Adapt them as needed for your specific implementation.
