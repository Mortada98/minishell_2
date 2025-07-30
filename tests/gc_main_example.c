/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_main_example.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 17:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/07/28 17:00:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* ========================================================================== */
/*                    MANUAL MEMORY MANAGEMENT (CURRENT)                     */
/* ========================================================================== */

void current_make_prompt_example(char **env)
{
    char *line;
    t_token *token;
    t_data *data = malloc(sizeof(t_data));  // ← Manual malloc
    t_command *cmd;

    while (1)
    {
        line = readline(promt());  // ← readline() uses malloc internally
        if (!line)
        {
            printf("exit\n");
            free(data);  // ← Manual free #1
            return;
        }
        
        // YOUR SELECTED CODE:
        if (line[0] == '\0')
        {
            free(line);    // ← Manual free #2 - YOU MUST REMEMBER THIS
            continue;
        }
        
        if (line[0] != '\0')
        {
            add_history(line);
            token = tokenize(line, &data);
            if (!token)
            {
                free(line);  // ← Manual free #3 - Another place to remember
                continue;
            }
            
            // ... more code ...
            
            if (logic_of_meta(token, &data) == false)
            {
                free(line);  // ← Manual free #4 - Yet another place!
                continue;
            }
            
            // ... more processing ...
            
            free_token(&token);  // ← Manual cleanup
            free_cmd(cmd);       // ← Manual cleanup
        }
        free(line);  // ← Manual free #5 - Final cleanup
    }
    free(data);  // ← Manual free #6
}

/* ========================================================================== */
/*                    GARBAGE COLLECTOR APPROACH                             */
/* ========================================================================== */

void gc_make_prompt_example(char **env)
{
    char *line;
    t_token *token;
    t_data *data = gc_calloc(1, sizeof(t_data));  // ← GC allocation

    while (1)
    {
        line = readline(promt());  // ← Still readline (external library)
        if (!line)
        {
            printf("exit\n");
            gc_cleanup();  // ← One cleanup call for everything
            return;
        }
        
        // Register readline's allocation with GC
        if (line)
            gc_register_external(line);
        
        // YOUR CODE BECOMES:
        if (line[0] == '\0')
        {
            // free(line);  ← REMOVED! No manual free needed
            continue;       // GC handles cleanup automatically
        }
        
        if (line[0] != '\0')
        {
            add_history(line);
            token = tokenize(line, &data);  // Uses GC internally
            if (!token)
            {
                // free(line);  ← REMOVED! No manual free needed
                continue;
            }
            
            // ... more code ...
            
            if (logic_of_meta(token, &data) == false)
            {
                // free(line);  ← REMOVED! No manual free needed
                continue;
            }
            
            // ... more processing ...
            
            // free_token(&token);  ← REMOVED! GC handles this
            // free_cmd(cmd);       ← REMOVED! GC handles this
        }
        // free(line);  ← REMOVED! GC handles this
        
        // Clean up allocations from this iteration only
        gc_cleanup_partial();
    }
    gc_cleanup();  // ← Final cleanup
}

/* ========================================================================== */
/*                        DETAILED GC EXPLANATION                            */
/* ========================================================================== */

/**
 * HOW GARBAGE COLLECTOR WORKS WITH YOUR EXAMPLE:
 */

void explain_gc_behavior(void)
{
    /*
     * STEP 1: ALLOCATION TRACKING
     * When you call gc_malloc/gc_calloc, the GC:
     * - Allocates memory using malloc()
     * - Adds the pointer to a linked list of tracked allocations
     * - Returns the pointer to you
     */
    
    char *line = readline("prompt> ");  // Regular malloc (external)
    gc_register_external(line);         // Tell GC to track this pointer
    
    /*
     * STEP 2: AUTOMATIC CLEANUP
     * When you call gc_cleanup_partial() or gc_cleanup():
     * - GC walks through its list of tracked pointers
     * - Calls free() on each pointer
     * - Removes the pointer from the tracking list
     */
    
    if (line[0] == '\0')
    {
        // OLD WAY: You must remember to free
        // free(line);
        
        // NEW WAY: Just continue, GC will handle it
        gc_cleanup_partial();  // Cleans up everything allocated in this iteration
        return;
    }
    
    /*
     * STEP 3: NESTED ALLOCATIONS
     * Functions like tokenize() that create many small allocations
     * become much cleaner:
     */
    
    // OLD WAY: tokenize() must manually manage memory
    t_token *token1 = malloc(sizeof(t_token));
    token1->av = malloc(strlen("hello") + 1);
    // ... if error occurs, must free both token1 and token1->av
    
    // NEW WAY: tokenize() uses GC
    t_token *token2 = gc_malloc(sizeof(t_token));
    token2->av = gc_strdup("hello");
    // ... if error occurs, gc_cleanup() frees everything automatically
}

/* ========================================================================== */
/*                        PRACTICAL BENEFITS                                 */
/* ========================================================================== */

void demonstrate_benefits(void)
{
    /*
     * BENEFIT 1: NO MEMORY LEAKS IN ERROR PATHS
     */
    
    // OLD WAY: Easy to leak memory
    void old_function(void)
    {
        char *buffer1 = malloc(100);
        char *buffer2 = malloc(200);
        
        if (some_error_condition())
        {
            free(buffer1);  // Must remember to free
            // free(buffer2);  // ← BUG: Forgot to free buffer2!
            return;
        }
        
        free(buffer1);
        free(buffer2);
    }
    
    // NEW WAY: No leaks possible
    void new_function(void)
    {
        char *buffer1 = gc_malloc(100);
        char *buffer2 = gc_malloc(200);
        
        if (some_error_condition())
        {
            gc_cleanup_partial();  // Frees everything automatically
            return;
        }
        
        gc_cleanup_partial();  // Frees everything automatically
    }
    
    /*
     * BENEFIT 2: SIMPLER ERROR HANDLING
     */
    
    // OLD WAY: Complex cleanup in tokenize()
    t_token *old_tokenize(char *line)
    {
        t_token *token = malloc(sizeof(t_token));
        if (!token)
            return NULL;
        
        token->av = malloc(strlen(line) + 1);
        if (!token->av)
        {
            free(token);  // Must cleanup token
            return NULL;
        }
        
        token->next = malloc(sizeof(t_token));
        if (!token->next)
        {
            free(token->av);  // Must cleanup av
            free(token);      // Must cleanup token
            return NULL;
        }
        
        return token;
    }
    
    // NEW WAY: Simple error handling
    t_token *new_tokenize(char *line)
    {
        t_token *token = gc_malloc(sizeof(t_token));
        if (!token)
            return NULL;  // GC will cleanup on next gc_cleanup_partial()
        
        token->av = gc_strdup(line);
        if (!token->av)
            return NULL;  // GC will cleanup everything
        
        token->next = gc_malloc(sizeof(t_token));
        if (!token->next)
            return NULL;  // GC will cleanup everything
        
        return token;
    }
}

/* ========================================================================== */
/*                        YOUR SPECIFIC CASE                                 */
/* ========================================================================== */

void your_code_transformation(void)
{
    /*
     * BEFORE (Manual Management):
     * You have 6+ places where you must remember to free(line)
     */
    
    char *line = readline("prompt> ");
    
    if (!line)
    {
        free(data);        // ← Must remember
        return;
    }
    
    if (line[0] == '\0')
    {
        free(line);        // ← Must remember (YOUR EXAMPLE)
        continue;
    }
    
    if (!token)
    {
        free(line);        // ← Must remember
        continue;
    }
    
    if (syntax_error)
    {
        free(line);        // ← Must remember
        continue;
    }
    
    // ... more free(line) calls ...
    
    /*
     * AFTER (GC Management):
     * One cleanup call handles everything
     */
    
    char *line2 = readline("prompt> ");
    if (line2)
        gc_register_external(line2);  // Register with GC
    
    if (!line2)
    {
        gc_cleanup();      // ← Cleans everything
        return;
    }
    
    if (line2[0] == '\0')
    {
        // No free needed!  // ← YOUR EXAMPLE SIMPLIFIED
        continue;
    }
    
    if (!token)
    {
        // No free needed!
        continue;
    }
    
    if (syntax_error)
    {
        // No free needed!
        continue;
    }
    
    // At end of loop iteration:
    gc_cleanup_partial();  // ← Cleans everything from this iteration
}

/* ========================================================================== */
/*                        INTEGRATION STRATEGY                               */
/* ========================================================================== */

void integration_steps(void)
{
    /*
     * STEP 1: Replace data structure allocation
     */
    
    // OLD:
    t_data *data = malloc(sizeof(t_data));
    
    // NEW:
    t_data *data = gc_calloc(1, sizeof(t_data));
    
    /*
     * STEP 2: Register external allocations
     */
    
    char *line = readline("prompt> ");
    if (line)
        gc_register_external(line);  // Track readline's allocation
    
    /*
     * STEP 3: Remove manual free() calls
     */
    
    if (line[0] == '\0')
    {
        // free(line);  ← Remove this line
        continue;
    }
    
    /*
     * STEP 4: Add cleanup points
     */
    
    // At end of each main loop iteration:
    gc_cleanup_partial();
    
    // At program exit:
    gc_cleanup();
}

/*
 * SUMMARY FOR YOUR SPECIFIC EXAMPLE:
 * 
 * Your code: if (line[0] == '\0') { free(line); continue; }
 * 
 * With GC:   if (line[0] == '\0') { continue; }
 * 
 * The GC automatically tracks and frees the 'line' pointer when
 * gc_cleanup_partial() is called at the end of the loop iteration.
 * 
 * Benefits:
 * 1. No memory leaks if you forget to free
 * 2. Simpler error handling
 * 3. Less code to maintain
 * 4. Automatic cleanup in all code paths
 * 5. No double-free errors possible
 */
