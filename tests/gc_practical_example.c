/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_practical_example.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 17:30:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/07/28 17:30:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* ========================================================================== */
/*                    HOW TO USE gc_register_external()                      */
/* ========================================================================== */

/**
 * EXPLANATION: gc_register_external() function
 * 
 * PURPOSE: Track external library allocations (like readline) in your GC
 * 
 * WHAT IT DOES:
 * 1. Takes a pointer allocated by external libraries (malloc, readline, etc.)
 * 2. Adds it to the GC's tracking list
 * 3. The pointer will be freed when gc_cleanup() or gc_cleanup_partial() is called
 */

void demonstrate_gc_register_external(void)
{
    /*
     * SCENARIO 1: readline() - Your exact case
     */
    
    char *line = readline("minishell> ");  // readline() uses malloc internally
    
    // Register this external allocation with GC
    if (line)
        gc_register_external(line);
    
    // Now the GC will automatically free 'line' when cleanup is called
    // You don't need to call free(line) anymore!
    
    /*
     * SCENARIO 2: Other external allocations
     */
    
    char *env_var = getenv("HOME");
    if (env_var)
    {
        char *home_copy = strdup(env_var);  // strdup uses malloc
        gc_register_external(home_copy);   // Track it in GC
    }
    
    /*
     * SCENARIO 3: Library functions that return allocated memory
     */
    
    char *current_dir = getcwd(NULL, 0);   // getcwd allocates memory
    if (current_dir)
        gc_register_external(current_dir); // Track it in GC
}

/* ========================================================================== */
/*                    YOUR SPECIFIC CASE: MAIN LOOP                          */
/* ========================================================================== */

/**
 * BEFORE: Your current main loop with manual memory management
 */
void before_gc_main_loop(char **env)
{
    char *line;
    t_token *token;
    t_data *data = malloc(sizeof(t_data));  // Manual allocation
    t_command *cmd;

    while (1)
    {
        line = readline(promt());
        if (!line)
        {
            printf("exit\n");
            free(data);  // Must remember to free
            return;
        }
        
        // YOUR SELECTED CODE:
        if (line[0] == '\0')
        {
            free(line);  // Must remember to free - EASY TO FORGET!
            continue;
        }
        
        if (line[0] != '\0')
        {
            add_history(line);
            token = tokenize(line, &data);
            if (!token)
            {
                free(line);  // Must remember to free - ANOTHER PLACE!
                continue;
            }
            
            if (logic_of_meta(token, &data) == false)
            {
                free(line);  // Must remember to free - YET ANOTHER PLACE!
                continue;
            }
            
            cmd = parsing_command(token, &data);
            if (!cmd)
            {
                free(line);  // Must remember to free - GETTING TEDIOUS!
                continue;
            }
            
            // Execute command...
            
            free_token(&token);  // Manual cleanup
            free_cmd(cmd);       // Manual cleanup
        }
        free(line);  // Must remember to free - FINAL PLACE!
    }
    free(data);  // Must remember to free
}

/**
 * AFTER: Using gc_register_external() 
 */
void after_gc_main_loop(char **env)
{
    char *line;
    t_token *token;
    t_data *data = gc_calloc(1, sizeof(t_data));  // GC allocation
    t_command *cmd;

    while (1)
    {
        line = readline(promt());
        if (!line)
        {
            printf("exit\n");
            gc_cleanup();  // One cleanup call handles everything!
            return;
        }
        
        // Register readline's allocation with GC
        gc_register_external(line);
        
        // YOUR CODE BECOMES MUCH SIMPLER:
        if (line[0] == '\0')
        {
            // No free(line) needed! GC will handle it
            continue;
        }
        
        if (line[0] != '\0')
        {
            add_history(line);
            token = tokenize(line, &data);  // Uses GC internally
            if (!token)
            {
                // No free(line) needed!
                continue;
            }
            
            if (logic_of_meta(token, &data) == false)
            {
                // No free(line) needed!
                continue;
            }
            
            cmd = parsing_command(token, &data);  // Uses GC internally
            if (!cmd)
            {
                // No free(line) needed!
                continue;
            }
            
            // Execute command...
            
            // No manual cleanup needed!
        }
        
        // Clean up all allocations from this iteration
        gc_cleanup_partial();
    }
    
    // Final cleanup
    gc_cleanup();
}

/* ========================================================================== */
/*                    STEP-BY-STEP INTEGRATION                               */
/* ========================================================================== */

/**
 * STEP 1: Modify your make_prompt() function
 */
void step1_modify_make_prompt(char **env)
{
    char *line;
    t_token *token;
    t_data *data = gc_calloc(1, sizeof(t_data));  // Change malloc to gc_calloc
    t_command *cmd;

    signal(SIGINT, my_handler);
    signal(SIGQUIT, SIG_IGN);
    
    while (1)
    {
        ft_memset(data, 0, sizeof(t_data));
        
        line = readline(promt());
        if (!line)
        {
            printf("exit\n");
            gc_cleanup();  // Replace free(data) with gc_cleanup()
            return;
        }
        
        // ADD THIS LINE:
        gc_register_external(line);  // Register readline allocation
        
        // REMOVE ALL free(line) calls from your code!
        if (line[0] == '\0')
        {
            // free(line);  ← REMOVE THIS LINE
            continue;
        }
        
        // Rest of your logic stays the same...
        
        // ADD THIS AT THE END OF THE LOOP:
        gc_cleanup_partial();  // Clean up iteration allocations
    }
    
    gc_cleanup();  // Final cleanup
}

/**
 * STEP 2: Update your tokenize() and parsing functions
 */
void step2_update_functions(void)
{
    /*
     * In tokenize(), parsing_command(), etc., replace:
     * 
     * OLD: malloc(size)
     * NEW: gc_malloc(size)
     * 
     * OLD: calloc(count, size)  
     * NEW: gc_calloc(count, size)
     * 
     * OLD: strdup(str)
     * NEW: gc_strdup(str)
     * 
     * REMOVE: All free() calls
     */
}

/* ========================================================================== */
/*                    TESTING YOUR GC IMPLEMENTATION                         */
/* ========================================================================== */

void test_gc_with_readline(void)
{
    printf("Testing GC with readline...\n");
    
    // Simulate your main loop
    for (int i = 0; i < 3; i++)
    {
        printf("\n--- Iteration %d ---\n", i + 1);
        
        // Simulate readline
        char *line = readline("test> ");
        if (!line)
            break;
            
        // Register with GC
        gc_register_external(line);
        
        // Simulate your processing
        if (line[0] == '\0')
        {
            printf("Empty line - continuing (no manual free needed)\n");
            continue;
        }
        
        printf("Processing line: %s\n", line);
        
        // Show GC status
        printf("Allocations before cleanup: %zu\n", gc_get_allocation_count());
        
        // Clean up this iteration
        gc_cleanup_partial();
        
        printf("Allocations after cleanup: %zu\n", gc_get_allocation_count());
    }
    
    // Final cleanup
    gc_cleanup();
    printf("Final allocations: %zu\n", gc_get_allocation_count());
}

/* ========================================================================== */
/*                    PRACTICAL USAGE SUMMARY                                */
/* ========================================================================== */

/*
 * HOW TO USE gc_register_external() IN YOUR CODE:
 * 
 * 1. WHENEVER you get a pointer from an external library:
 *    char *line = readline("prompt> ");
 *    if (line) gc_register_external(line);
 * 
 * 2. REMOVE all manual free() calls for that pointer:
 *    // free(line);  ← Delete these lines
 * 
 * 3. ADD cleanup calls at appropriate points:
 *    gc_cleanup_partial();  // End of loop iteration
 *    gc_cleanup();          // Program exit
 * 
 * 4. YOUR SPECIFIC EXAMPLE BECOMES:
 *    if (line[0] == '\0')
 *    {
 *        // free(line);  ← REMOVE THIS
 *        continue;       ← Just continue, GC handles the rest
 *    }
 * 
 * BENEFITS:
 * ✅ No memory leaks
 * ✅ No double-free errors  
 * ✅ Simpler error handling
 * ✅ Less code to maintain
 * ✅ Automatic cleanup in all code paths
 */
