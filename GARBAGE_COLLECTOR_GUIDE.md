# Garbage Collector Integration Guide for Minishell

## Overview
This garbage collector provides automatic memory management for your minishell project. It tracks all allocations and can free them automatically.

## Key Functions

### Core Functions
- `gc_malloc(size)` - Replacement for `malloc()`
- `gc_calloc(count, size)` - Replacement for `calloc()`
- `gc_strdup(str)` - Replacement for `strdup()` or `ft_strdup()`
- `gc_free(ptr)` - Replacement for `free()` (optional, as cleanup handles this)
- `gc_cleanup()` - Frees ALL tracked memory (use at program exit)
- `gc_cleanup_partial()` - Frees all tracked memory but keeps gc active (use between commands)

### Utility Functions
- `gc_get_allocation_count()` - Returns number of tracked allocations
- `gc_print_allocations()` - Debug function to print all tracked pointers

## Integration Strategy

### 1. Replace Memory Allocation Functions

**Before:**
```c
char *str = malloc(100);
char *dup = ft_strdup("hello");
t_token *token = calloc(1, sizeof(t_token));
```

**After:**
```c
char *str = gc_malloc(100);
char *dup = gc_strdup("hello");
t_token *token = gc_calloc(1, sizeof(t_token));
```

### 2. Remove Manual Free Calls

**Before:**
```c
free(token->av);
free(token);
```

**After:**
```c
// No manual free needed - gc_cleanup() or gc_cleanup_partial() handles it
```

### 3. Strategic Cleanup Points

#### Option A: Cleanup After Each Command (Recommended)
```c
while (1)
{
    line = readline("minishell> ");
    if (!line) break;
    
    // Process command using gc_malloc, gc_strdup, etc.
    tokens = tokenize(line, &data);
    commands = parsing_command(tokens, &data);
    execute_command(commands, environ, &data);
    
    // Clean up all allocations from this iteration
    gc_cleanup_partial();
    
    free(line); // readline uses regular malloc
}
gc_cleanup(); // Final cleanup
```

#### Option B: Cleanup Only on Exit
```c
while (1)
{
    // ... process commands with garbage collector
}
gc_cleanup(); // Clean everything at the end
```

### 4. Signal Handler Integration
```c
void cleanup_handler(int sig)
{
    gc_cleanup();
    exit(sig == SIGINT ? 130 : 1);
}

int main(void)
{
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    // ... rest of main
}
```

## Files to Modify

### High Priority (Core Memory Management)
1. **Handle_token.c** - Token creation and manipulation
   - Replace `ft_calloc` in `creat_token()` with `gc_calloc`
   - Replace `ft_strdup` with `gc_strdup`

2. **Handle_command.c** - Command structure management
   - Replace `ft_calloc` in `create_command()` with `gc_calloc`
   - Replace `ft_strdup` calls with `gc_strdup`
   - Modify `append_arg()` to use `gc_calloc`

3. **main.c** - Main execution loop
   - Add `gc_cleanup_partial()` after each command execution
   - Add `gc_cleanup()` before program exit

### Medium Priority
4. **Helper.c** - Array management
   - Modify `append_arg()` to use garbage collector
   - Remove manual free calls in functions that will use gc

5. **utils.c**, **tools.c** - Utility functions
   - Replace `malloc` calls with `gc_malloc`
   - Replace `ft_strdup` calls with `gc_strdup`

### Low Priority
6. **expand_env** functions - Environment variable expansion
   - Use `gc_malloc` for result buffers
   - Use `gc_strdup` for duplicated strings

## Example Modifications

### 1. Modified `creat_token` function:
```c
// OLD VERSION
t_token *creat_token(char *line, t_token_type type, bool should_join)
{
    t_token *new_token = ft_calloc(1, sizeof(t_token));
    if (!new_token) return NULL;
    new_token->av = ft_strdup(line);
    // ...
}

// NEW VERSION WITH GC
t_token *creat_token(char *line, t_token_type type, bool should_join)
{
    t_token *new_token = gc_calloc(1, sizeof(t_token));
    if (!new_token) return NULL;
    new_token->av = gc_strdup(line);
    // ...
}
```

### 2. Modified main loop:
```c
// Add to main function
while (1)
{
    char *prompt_str = promt();
    line = readline(prompt_str);
    free(prompt_str); // promt() might still use malloc
    
    if (!line) break;
    if (*line) add_history(line);
    
    // Tokenize and parse using GC functions
    t_data data = {0};
    t_token *tokens = tokenize(line, &data);
    t_command *commands = parsing_command(tokens, &data);
    
    // Execute
    execute_command(commands, environ, &data);
    
    // Cleanup this iteration's allocations
    gc_cleanup_partial();
    
    free(line); // readline allocates with regular malloc
}

// Final cleanup
gc_cleanup();
```

## Benefits

1. **No Memory Leaks**: Automatic cleanup ensures no memory is leaked
2. **Simplified Code**: No need to track and free individual allocations
3. **Error Recovery**: If execution stops unexpectedly, cleanup is still possible
4. **Debugging**: Easy to track allocation count and find memory issues

## Important Notes

1. **Don't mix regular malloc/free with gc functions** for the same memory
2. **External libraries** (like readline) still use regular malloc - free those manually
3. **Use gc_cleanup_partial() between commands** for better memory efficiency
4. **Use gc_cleanup() only at program exit** or when completely done
5. **Signal handlers should call gc_cleanup()** for clean exits

## Testing

Run your shell and check allocations:
```c
printf("Allocations before command: %zu\n", gc_get_allocation_count());
// ... execute command ...
printf("Allocations after command: %zu\n", gc_get_allocation_count());
gc_cleanup_partial();
printf("Allocations after cleanup: %zu\n", gc_get_allocation_count());
```

The count should return to 0 or a small baseline after `gc_cleanup_partial()`.
