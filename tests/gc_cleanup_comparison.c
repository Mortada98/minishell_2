/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_cleanup_comparison.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:45:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/07/28 18:45:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* ========================================================================== */
/*                    gc_cleanup_partial() vs gc_cleanup()                   */
/* ========================================================================== */

/**
 * DETAILED COMPARISON OF THE TWO CLEANUP FUNCTIONS
 */

void demonstrate_cleanup_differences(void)
{
    printf("=== GC CLEANUP FUNCTIONS COMPARISON ===\n\n");
    
    // SETUP: Allocate some memory
    printf("1. Setting up test allocations...\n");
    char *ptr1 = gc_malloc(100);
    char *ptr2 = gc_malloc(200);
    char *ptr3 = gc_malloc(300);
    
    printf("   Allocations created: %zu\n", gc_get_allocation_count());
    printf("   GC system status: %s\n\n", g_gc ? "ACTIVE" : "INACTIVE");
    
    // TEST gc_cleanup_partial()
    printf("2. Testing gc_cleanup_partial()...\n");
    printf("   Before: %zu allocations\n", gc_get_allocation_count());
    
    gc_cleanup_partial();
    
    printf("   After partial cleanup:\n");
    printf("   - Allocations: %zu\n", gc_get_allocation_count());
    printf("   - GC system: %s\n", g_gc ? "STILL ACTIVE" : "DESTROYED");
    printf("   - Can allocate again: ");
    
    char *new_ptr = gc_malloc(50);
    if (new_ptr)
        printf("YES ✅\n\n");
    else
        printf("NO ❌\n\n");
    
    // TEST gc_cleanup()
    printf("3. Testing gc_cleanup()...\n");
    printf("   Before: %zu allocations\n", gc_get_allocation_count());
    
    gc_cleanup();
    
    printf("   After full cleanup:\n");
    printf("   - Allocations: %zu\n", gc_get_allocation_count());
    printf("   - GC system: %s\n", g_gc ? "STILL ACTIVE" : "DESTROYED");
    printf("   - Can allocate again: ");
    
    char *another_ptr = gc_malloc(75);
    if (another_ptr)
        printf("YES (GC reinitializes) ✅\n\n");
    else
        printf("NO ❌\n\n");
}

/* ========================================================================== */
/*                    WHEN TO USE gc_cleanup_partial()                       */
/* ========================================================================== */

/**
 * USE gc_cleanup_partial() FOR:
 * - End of loop iterations
 * - Cleaning up temporary allocations
 * - When you want to continue using GC
 */

void example_partial_cleanup_usage(void)
{
    printf("=== WHEN TO USE gc_cleanup_partial() ===\n\n");
    
    printf("SCENARIO 1: Main shell loop iterations\n");
    for (int i = 1; i <= 3; i++)
    {
        printf("--- Loop iteration %d ---\n", i);
        
        // Simulate command processing
        char *line = gc_malloc(256);
        char *token1 = gc_malloc(64);
        char *token2 = gc_malloc(64);
        
        printf("Allocated memory for iteration: %zu items\n", gc_get_allocation_count());
        
        // Process command (your shell logic here)
        printf("Processing command...\n");
        
        // Clean up THIS ITERATION's allocations
        gc_cleanup_partial();
        printf("After cleanup: %zu items (ready for next iteration)\n\n", gc_get_allocation_count());
        
        // Loop continues - GC is still active and ready
    }
    
    printf("SCENARIO 2: Function-level cleanup\n");
    function_with_temporary_allocations();
    printf("After function: %zu allocations remain\n\n", gc_get_allocation_count());
    
    printf("SCENARIO 3: Error recovery\n");
    simulate_error_with_partial_cleanup();
    printf("After error recovery: GC still functional\n\n");
}

void function_with_temporary_allocations(void)
{
    printf("Inside function - creating temporary data...\n");
    
    char *temp1 = gc_malloc(128);
    char *temp2 = gc_malloc(256);
    char *temp3 = gc_malloc(512);
    
    printf("Function allocated: %zu items\n", gc_get_allocation_count());
    
    // Do some processing...
    
    // Clean up only the temporary allocations from this function
    gc_cleanup_partial();
    printf("Function cleaned up its allocations\n");
}

void simulate_error_with_partial_cleanup(void)
{
    printf("Simulating error scenario...\n");
    
    char *buffer = gc_malloc(1024);
    if (!buffer)
    {
        printf("Error: Could not allocate buffer\n");
        gc_cleanup_partial();  // Clean up any partial allocations
        return;
    }
    
    // Simulate error during processing
    printf("Error occurred during processing!\n");
    
    // Clean up and recover - GC stays active for retry
    gc_cleanup_partial();
    printf("Error cleanup complete - can retry operation\n");
}

/* ========================================================================== */
/*                    WHEN TO USE gc_cleanup()                               */
/* ========================================================================== */

/**
 * USE gc_cleanup() FOR:
 * - Program exit
 * - Complete shutdown
 * - When you're done with GC entirely
 */

void example_full_cleanup_usage(void)
{
    printf("=== WHEN TO USE gc_cleanup() ===\n\n");
    
    printf("SCENARIO 1: Program exit\n");
    simulate_program_exit();
    
    printf("SCENARIO 2: Module shutdown\n");
    simulate_module_shutdown();
    
    printf("SCENARIO 3: Critical error\n");
    simulate_critical_error();
}

void simulate_program_exit(void)
{
    printf("Program is exiting...\n");
    
    // Allocate some data
    char *final_data = gc_malloc(100);
    printf("Final allocations: %zu\n", gc_get_allocation_count());
    
    // Program ending - destroy everything
    gc_cleanup();
    printf("All memory cleaned up. GC destroyed. Program can exit safely.\n\n");
}

void simulate_module_shutdown(void)
{
    printf("Shutting down minishell module...\n");
    
    char *module_data = gc_malloc(200);
    printf("Module allocations: %zu\n", gc_get_allocation_count());
    
    // Module shutting down completely
    gc_cleanup();
    printf("Module shutdown complete. All resources freed.\n\n");
}

void simulate_critical_error(void)
{
    printf("Critical error occurred - emergency shutdown...\n");
    
    char *error_data = gc_malloc(300);
    printf("Allocations before emergency cleanup: %zu\n", gc_get_allocation_count());
    
    // Critical error - clean everything and exit
    gc_cleanup();
    printf("Emergency cleanup complete. Ready for exit.\n\n");
}

/* ========================================================================== */
/*                    YOUR MINISHELL USAGE PATTERNS                          */
/* ========================================================================== */

void your_minishell_usage_examples(void)
{
    printf("=== YOUR MINISHELL USAGE PATTERNS ===\n\n");
    
    printf("PATTERN 1: Main shell loop\n");
    minishell_main_loop_example();
    
    printf("PATTERN 2: Signal handlers\n");
    signal_handler_example();
    
    printf("PATTERN 3: Command execution\n");
    command_execution_example();
}

void minishell_main_loop_example(void)
{
    printf("void make_prompt(char **env)\n");
    printf("{\n");
    printf("    while (1) {\n");
    printf("        char *line = readline(\"minishell> \");\n");
    printf("        if (!line) {\n");
    printf("            gc_cleanup();  // ← Full cleanup before exit\n");
    printf("            return;\n");
    printf("        }\n");
    printf("        \n");
    printf("        gc_register_external(line);\n");
    printf("        \n");
    printf("        // Process command...\n");
    printf("        token = tokenize(line, &data);\n");
    printf("        cmd = parsing_command(token, &data);\n");
    printf("        execute_command(cmd, env, &data);\n");
    printf("        \n");
    printf("        gc_cleanup_partial();  // ← Partial cleanup each iteration\n");
    printf("    }\n");
    printf("}\n\n");
}

void signal_handler_example(void)
{
    printf("void signal_handler(int sig)\n");
    printf("{\n");
    printf("    if (sig == SIGINT) {\n");
    printf("        write(1, \"\\n\", 1);\n");
    printf("        gc_cleanup();  // ← Full cleanup before exit\n");
    printf("        exit(130);\n");
    printf("    }\n");
    printf("}\n\n");
}

void command_execution_example(void)
{
    printf("int execute_builtin(t_command *cmd)\n");
    printf("{\n");
    printf("    char *temp_buffer = gc_malloc(256);\n");
    printf("    if (!temp_buffer) {\n");
    printf("        gc_cleanup_partial();  // ← Cleanup on error\n");
    printf("        return (1);\n");
    printf("    }\n");
    printf("    \n");
    printf("    // Process builtin command...\n");
    printf("    \n");
    printf("    // Don't cleanup here - let main loop handle it\n");
    printf("    return (0);\n");
    printf("}\n\n");
}

/* ========================================================================== */
/*                    PRACTICAL DECISION TREE                                */
/* ========================================================================== */

void cleanup_decision_tree(void)
{
    printf("=== WHICH CLEANUP FUNCTION TO USE? ===\n\n");
    
    printf("ASK YOURSELF:\n\n");
    
    printf("1. Is the program/module ending completely?\n");
    printf("   YES → use gc_cleanup()\n");
    printf("   NO  → continue to question 2\n\n");
    
    printf("2. Do you want to continue using GC after cleanup?\n");
    printf("   YES → use gc_cleanup_partial()\n");
    printf("   NO  → use gc_cleanup()\n\n");
    
    printf("3. Are you in a loop that will repeat?\n");
    printf("   YES → use gc_cleanup_partial()\n");
    printf("   NO  → continue to question 4\n\n");
    
    printf("4. Is this cleanup after an error?\n");
    printf("   RECOVERABLE ERROR → use gc_cleanup_partial()\n");
    printf("   FATAL ERROR       → use gc_cleanup()\n\n");
    
    printf("EXAMPLES:\n");
    printf("✅ gc_cleanup_partial(): End of shell command, function cleanup, error recovery\n");
    printf("✅ gc_cleanup():         Program exit, signal handlers, fatal errors\n\n");
}

/* ========================================================================== */
/*                    SUMMARY OF DIFFERENCES                                 */
/* ========================================================================== */

/*
 * gc_cleanup_partial():
 * =====================
 * WHAT IT DOES:
 * - Frees all currently tracked memory
 * - Resets allocation count to 0
 * - Keeps GC system active and ready
 * 
 * WHEN TO USE:
 * - End of loop iterations (your main shell loop)
 * - After processing each command
 * - Function-level cleanup
 * - Error recovery where you want to continue
 * - Temporary allocation cleanup
 * 
 * AFTER CALLING:
 * - Can immediately use gc_malloc() again
 * - GC system remains initialized
 * - Ready for next batch of allocations
 * 
 * YOUR USAGE:
 * while (1) {
 *     // allocate and process
 *     gc_cleanup_partial();  // ← Clean this iteration
 * }
 * 
 * 
 * gc_cleanup():
 * =============
 * WHAT IT DOES:
 * - Frees all currently tracked memory
 * - Destroys the entire GC system
 * - Sets g_gc = NULL
 * 
 * WHEN TO USE:
 * - Program exit
 * - Module shutdown
 * - Signal handlers that exit
 * - Fatal errors
 * - When completely done with GC
 * 
 * AFTER CALLING:
 * - GC system is destroyed
 * - Next gc_malloc() will reinitialize GC
 * - Usually called before exit()
 * 
 * YOUR USAGE:
 * if (!line) {
 *     gc_cleanup();  // ← Destroy everything before exit
 *     return;
 * }
 */
