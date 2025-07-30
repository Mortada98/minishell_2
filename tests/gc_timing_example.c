/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_timing_example.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/07/28 18:00:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* ========================================================================== */
/*                    WHEN DOES GC FREE MEMORY? VISUAL DEMO                  */
/* ========================================================================== */

void demonstrate_gc_timing(void)
{
    printf("=== GC TIMING DEMONSTRATION ===\n\n");
    
    // Simulate your main loop iterations
    for (int i = 1; i <= 3; i++)
    {
        printf("--- Loop Iteration %d ---\n", i);
        
        // STEP 1: ALLOCATION & TRACKING
        printf("1. Allocating memory...\n");
        char *line = malloc(10);  // Simulate readline
        strcpy(line, i == 2 ? "" : "command");  // Empty on iteration 2
        
        printf("   Memory allocated at: %p\n", (void*)line);
        printf("   GC allocations before register: %zu\n", gc_get_allocation_count());
        
        gc_register_external(line);
        printf("   GC allocations after register: %zu\n", gc_get_allocation_count());
        
        // STEP 2: YOUR PROCESSING (No freeing!)
        printf("2. Processing line: '%s'\n", line);
        
        if (line[0] == '\0')  // This is your exact condition
        {
            printf("   Line is empty - continuing\n");
            printf("   Memory at %p is STILL ALLOCATED!\n", (void*)line);
            printf("   GC still tracking: %zu allocations\n", gc_get_allocation_count());
            
            // Your continue statement would jump to step 3
        }
        else
        {
            printf("   Processing non-empty line...\n");
            printf("   Memory at %p is STILL ALLOCATED!\n", (void*)line);
            printf("   GC still tracking: %zu allocations\n", gc_get_allocation_count());
        }
        
        // STEP 3: CLEANUP (This is when freeing happens!)
        printf("3. Calling gc_cleanup_partial()...\n");
        printf("   Memory BEFORE cleanup: %zu allocations\n", gc_get_allocation_count());
        
        gc_cleanup_partial();  // ← THIS IS WHEN line GETS FREED
        
        printf("   Memory AFTER cleanup: %zu allocations\n", gc_get_allocation_count());
        printf("   Memory at %p has been FREED!\n\n", (void*)line);
    }
    
    printf("=== END DEMONSTRATION ===\n");
}

/* ========================================================================== */
/*                    YOUR EXACT CODE WITH TIMING COMMENTS                   */
/* ========================================================================== */

void your_code_with_timing_explanation(char **env)
{
    char *line;
    t_token *token;
    t_data *data = gc_calloc(1, sizeof(t_data));
    t_command *cmd;

    while (1)
    {
        // ==================== ALLOCATION PHASE ====================
        printf("DEBUG: About to call readline...\n");
        line = readline(promt());  // Memory allocated by readline
        printf("DEBUG: readline returned: %p\n", (void*)line);
        
        if (!line)
        {
            gc_cleanup();  // Free everything before exit
            return;
        }
        
        printf("DEBUG: Registering with GC...\n");
        gc_register_external(line);  // Tell GC to track this memory
        printf("DEBUG: GC now tracking %zu allocations\n", gc_get_allocation_count());
        
        // ==================== PROCESSING PHASE ====================
        // IMPORTANT: No memory is freed in this section!
        
        if (line[0] == '\0')  // YOUR CONDITION
        {
            printf("DEBUG: Empty line detected\n");
            printf("DEBUG: Memory at %p is STILL ALLOCATED\n", (void*)line);
            printf("DEBUG: GC still tracking %zu allocations\n", gc_get_allocation_count());
            printf("DEBUG: About to continue (jump to cleanup phase)\n");
            
            // When you 'continue', execution jumps to the cleanup phase below
            // The memory is NOT freed here!
            continue;
        }
        
        if (line[0] != '\0')
        {
            printf("DEBUG: Processing non-empty line: '%s'\n", line);
            printf("DEBUG: Memory at %p is STILL ALLOCATED\n", (void*)line);
            
            add_history(line);
            token = tokenize(line, &data);  // Creates more GC allocations
            
            if (!token)
            {
                printf("DEBUG: No tokens, but memory still allocated\n");
                printf("DEBUG: GC tracking %zu allocations\n", gc_get_allocation_count());
                // Again, 'continue' jumps to cleanup - no freeing here
                continue;
            }
            
            // More processing...
            cmd = parsing_command(token, &data);  // More GC allocations
            
            printf("DEBUG: After parsing, GC tracking %zu allocations\n", gc_get_allocation_count());
            
            // Execute command...
        }
        
        // ==================== CLEANUP PHASE ====================
        // THIS IS WHERE ALL MEMORY GETS FREED!
        
        printf("DEBUG: About to call gc_cleanup_partial()...\n");
        printf("DEBUG: Memory before cleanup: %zu allocations\n", gc_get_allocation_count());
        
        gc_cleanup_partial();  // ← FREES line + all other GC allocations
        
        printf("DEBUG: Memory after cleanup: %zu allocations\n", gc_get_allocation_count());
        printf("DEBUG: Memory at %p has been FREED!\n", (void*)line);
        printf("DEBUG: End of loop iteration\n\n");
        
        // Loop repeats...
    }
}

/* ========================================================================== */
/*                    WHY THIS WORKS BETTER THAN MANUAL FREE                */
/* ========================================================================== */

void compare_manual_vs_gc(void)
{
    printf("=== MANUAL vs GC COMPARISON ===\n\n");
    
    // MANUAL MEMORY MANAGEMENT (Your current approach)
    printf("MANUAL APPROACH:\n");
    printf("- You must remember to free(line) in EVERY code path\n");
    printf("- Easy to forget and cause memory leaks\n");
    printf("- Complex error handling\n");
    printf("- Example paths where you must free:\n");
    printf("  1. if (line[0] == '\\0') { free(line); continue; }\n");
    printf("  2. if (!token) { free(line); continue; }\n");
    printf("  3. if (syntax_error) { free(line); continue; }\n");
    printf("  4. At end of loop: free(line);\n");
    printf("  5. On error: free(line);\n\n");
    
    // GC APPROACH
    printf("GC APPROACH:\n");
    printf("- Register once: gc_register_external(line)\n");
    printf("- ONE cleanup call handles ALL memory: gc_cleanup_partial()\n");
    printf("- No memory leaks possible\n");
    printf("- Simple error handling\n");
    printf("- Example - all paths automatically handled:\n");
    printf("  1. if (line[0] == '\\0') { continue; }  // No free needed\n");
    printf("  2. if (!token) { continue; }           // No free needed\n");
    printf("  3. if (syntax_error) { continue; }     // No free needed\n");
    printf("  4. gc_cleanup_partial();               // Frees everything\n\n");
}

/* ========================================================================== */
/*                    ANSWER TO YOUR QUESTION                                */
/* ========================================================================== */

/*
 * QUESTION: "How does gc free automatically when line[0] == '\0'?"
 * 
 * ANSWER: It doesn't free automatically at that moment!
 * 
 * HERE'S THE ACTUAL SEQUENCE:
 * 
 * 1. line = readline("prompt> ");      // Memory allocated
 * 2. gc_register_external(line);       // Memory tracked by GC
 * 3. if (line[0] == '\0') continue;    // NO freeing happens here!
 * 4. gc_cleanup_partial();             // THIS is when memory gets freed
 * 
 * THE MAGIC IS:
 * - GC "remembers" all pointers you register
 * - When you call cleanup, it frees ALL remembered pointers
 * - You don't need to manually free in each code path
 * 
 * TIMING:
 * - Register: Happens once per allocation
 * - Free: Happens once per cleanup call (usually end of loop)
 * 
 * BENEFIT:
 * - No matter which code path you take (empty line, error, success)
 * - The cleanup call at the end handles ALL memory
 * - Impossible to leak memory or forget to free
 */
