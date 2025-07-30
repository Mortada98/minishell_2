/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_null_check_examples.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:30:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/07/28 18:30:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* ========================================================================== */
/*                    WHY YOU STILL NEED NULL CHECKS WITH GC                 */
/* ========================================================================== */

/**
 * ANSWER: YES, you still need NULL checks with gc_malloc()!
 * 
 * REASONS:
 * 1. gc_malloc() can fail if system runs out of memory
 * 2. gc_malloc() calls regular malloc() internally
 * 3. If malloc() fails, gc_malloc() returns NULL
 * 4. Using NULL pointer = SEGMENTATION FAULT
 */

void demonstrate_why_null_checks_needed(void)
{
    printf("=== WHY NULL CHECKS ARE STILL NEEDED ===\n\n");
    
    // BAD: No NULL check - DANGEROUS!
    printf("BAD EXAMPLE (can crash):\n");
    char *bad_example = gc_malloc(100);
    // What if gc_malloc failed? bad_example could be NULL!
    strcpy(bad_example, "Hello");  // ← SEGFAULT if bad_example is NULL!
    printf("This could crash: %s\n\n", bad_example);
    
    // GOOD: With NULL check - SAFE!
    printf("GOOD EXAMPLE (safe):\n");
    char *good_example = gc_malloc(100);
    if (!good_example)
    {
        printf("Error: Memory allocation failed\n");
        return;  // Exit safely - GC will clean up on next gc_cleanup()
    }
    strcpy(good_example, "Hello");  // Safe - we know it's not NULL
    printf("This is safe: %s\n\n", good_example);
}

/* ========================================================================== */
/*                    COMPARISON: MANUAL vs GC ERROR HANDLING                */
/* ========================================================================== */

void compare_error_handling(void)
{
    printf("=== MANUAL vs GC ERROR HANDLING ===\n\n");
    
    // MANUAL MEMORY MANAGEMENT
    printf("MANUAL APPROACH:\n");
    char *manual1 = malloc(100);
    if (!manual1)
    {
        printf("Error: malloc failed\n");
        return;  // Must return - no cleanup needed yet
    }
    
    char *manual2 = malloc(200);
    if (!manual2)
    {
        printf("Error: second malloc failed\n");
        free(manual1);  // ← Must remember to free manual1!
        return;
    }
    
    char *manual3 = malloc(300);
    if (!manual3)
    {
        printf("Error: third malloc failed\n");
        free(manual1);  // ← Must remember to free manual1!
        free(manual2);  // ← Must remember to free manual2!
        return;
    }
    
    // Use variables...
    free(manual1);  // ← Must remember to free all!
    free(manual2);
    free(manual3);
    printf("Manual: Complex cleanup required\n\n");
    
    // GC APPROACH
    printf("GC APPROACH:\n");
    char *gc1 = gc_malloc(100);
    if (!gc1)
    {
        printf("Error: gc_malloc failed\n");
        return;  // GC will cleanup on next gc_cleanup() call
    }
    
    char *gc2 = gc_malloc(200);
    if (!gc2)
    {
        printf("Error: second gc_malloc failed\n");
        return;  // GC will cleanup gc1 automatically!
    }
    
    char *gc3 = gc_malloc(300);
    if (!gc3)
    {
        printf("Error: third gc_malloc failed\n");
        return;  // GC will cleanup gc1 and gc2 automatically!
    }
    
    // Use variables...
    // No manual free needed - gc_cleanup() handles everything!
    printf("GC: Simple cleanup - gc_cleanup() handles all\n\n");
}

/* ========================================================================== */
/*                    YOUR BUILTIN FUNCTIONS WITH GC                         */
/* ========================================================================== */

/**
 * EXAMPLE: cd_home function converted to use GC
 */
static int gc_cd_home(char ***env)
{
    char *home;
    char *current_pwd;

    home = getenv("HOME");  // This can return NULL
    if (!home)
    {
        printf("minishell: cd: HOME not set\n");
        return (1);
    }

    current_pwd = getcwd(NULL, 0);  // This can return NULL
    if (!current_pwd)  // ← Still need NULL check!
    {
        perror("minishell: getcwd");
        return (1);
    }
    
    // Register with GC instead of manual free
    gc_register_external(current_pwd);

    if (chdir(home) != 0)
    {
        perror("minishell: cd");
        // No manual free needed - GC will handle current_pwd
        return (1);
    }

    if (current_pwd)  // This check is still good practice
    {
        update_pwd_vars(current_pwd, home, env);
        // No manual free needed - GC will handle current_pwd
    }
    return (0);
}

/**
 * EXAMPLE: pwd function converted to use GC
 */
int gc_builtin_pwd(t_command *cmd)
{
    char *pwd;

    (void)cmd; // pwd takes no arguments

    pwd = getcwd(NULL, 0);  // This can return NULL
    if (!pwd)  // ← Still need NULL check!
    {
        perror("minishell: pwd");
        return (1);
    }

    // Register with GC instead of manual free
    gc_register_external(pwd);

    printf("%s\n", pwd);
    // No manual free needed - GC will handle pwd
    return (0);
}

/* ========================================================================== */
/*                    PATTERN: GC ALLOCATION WITH ERROR HANDLING             */
/* ========================================================================== */

/**
 * STANDARD PATTERN for GC allocations with error handling
 */
void gc_allocation_pattern_examples(void)
{
    printf("=== STANDARD GC ALLOCATION PATTERNS ===\n\n");
    
    // PATTERN 1: Simple allocation
    printf("Pattern 1 - Simple allocation:\n");
    char *buffer = gc_malloc(256);
    if (!buffer)
    {
        printf("Error: Memory allocation failed\n");
        return;  // GC will cleanup on next gc_cleanup()
    }
    printf("Buffer allocated successfully\n");
    
    // PATTERN 2: String duplication
    printf("\nPattern 2 - String duplication:\n");
    char *str = gc_strdup("Hello, World!");
    if (!str)
    {
        printf("Error: String duplication failed\n");
        return;  // GC will cleanup buffer and any other allocations
    }
    printf("String duplicated: %s\n", str);
    
    // PATTERN 3: External function result
    printf("\nPattern 3 - External function result:\n");
    char *cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        perror("getcwd failed");
        return;  // GC will cleanup buffer and str
    }
    gc_register_external(cwd);  // Register external allocation
    printf("Current directory: %s\n", cwd);
    
    // PATTERN 4: Array allocation
    printf("\nPattern 4 - Array allocation:\n");
    char **array = gc_calloc(10, sizeof(char *));
    if (!array)
    {
        printf("Error: Array allocation failed\n");
        return;  // GC will cleanup all previous allocations
    }
    printf("Array allocated successfully\n");
    
    printf("\nAll allocations will be cleaned up by gc_cleanup()\n");
}

/* ========================================================================== */
/*                    WHEN NULL CHECKS ARE MOST IMPORTANT                    */
/* ========================================================================== */

void critical_null_check_scenarios(void)
{
    printf("=== CRITICAL NULL CHECK SCENARIOS ===\n\n");
    
    // SCENARIO 1: String operations
    printf("Scenario 1 - String operations:\n");
    char *name = gc_strdup("user");
    if (!name)
    {
        printf("Error: Cannot allocate name\n");
        return;
    }
    // Safe to use name now
    printf("Hello, %s!\n", name);
    
    // SCENARIO 2: File operations
    printf("\nScenario 2 - File operations:\n");
    char *filename = gc_malloc(256);
    if (!filename)
    {
        printf("Error: Cannot allocate filename buffer\n");
        return;
    }
    snprintf(filename, 256, "/tmp/minishell_%d", getpid());
    printf("Temp file: %s\n", filename);
    
    // SCENARIO 3: Environment variables
    printf("\nScenario 3 - Environment variables:\n");
    char *home = getenv("HOME");
    if (!home)  // getenv can return NULL
    {
        printf("Error: HOME not set\n");
        return;
    }
    
    char *home_copy = gc_strdup(home);
    if (!home_copy)
    {
        printf("Error: Cannot copy HOME value\n");
        return;
    }
    printf("Home directory: %s\n", home_copy);
    
    // SCENARIO 4: Command parsing
    printf("\nScenario 4 - Command parsing:\n");
    char *command = gc_malloc(128);
    if (!command)
    {
        printf("Error: Cannot allocate command buffer\n");
        return;
    }
    strcpy(command, "ls -la");
    printf("Command to execute: %s\n", command);
}

/* ========================================================================== */
/*                    PRACTICAL ANSWER TO YOUR QUESTION                      */
/* ========================================================================== */

/*
 * QUESTION: "When char *test = gc_malloc do i need to check if line == NULL for protection?"
 * 
 * ANSWER: YES! Always check for NULL after gc_malloc()
 * 
 * CORRECT PATTERN:
 * 
 * char *test = gc_malloc(size);
 * if (!test)
 * {
 *     // Handle error
 *     return (error_code);
 * }
 * // Safe to use test now
 * 
 * WHY:
 * 1. gc_malloc() can fail just like malloc()
 * 2. Using NULL pointer causes segmentation fault
 * 3. GC doesn't protect against NULL pointer usage
 * 4. GC only helps with memory cleanup, not allocation failure
 * 
 * BENEFITS OF GC WITH NULL CHECKS:
 * ✅ Still need to check for allocation failure
 * ✅ But no need to manually free on error paths
 * ✅ GC handles cleanup automatically
 * ✅ Simpler error handling code
 * ✅ No memory leaks in error scenarios
 * 
 * EXAMPLE IN YOUR BUILTIN:
 * 
 * // OLD (manual):
 * current_pwd = getcwd(NULL, 0);
 * if (!current_pwd) return (1);
 * // ... use current_pwd ...
 * free(current_pwd);  ← Must remember this
 * 
 * // NEW (GC):
 * current_pwd = getcwd(NULL, 0);
 * if (!current_pwd) return (1);  ← Still need this check!
 * gc_register_external(current_pwd);
 * // ... use current_pwd ...
 * // No free needed - GC handles it
 */
