/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_gc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 10:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/07/28 10:00:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void test_garbage_collector(void)
{
    printf("=== Garbage Collector Test ===\n");
    
    // Initial state
    printf("Initial allocations: %zu\n", gc_get_allocation_count());
    
    // Test gc_malloc
    char *ptr1 = gc_malloc(100);
    printf("After gc_malloc(100): %zu allocations\n", gc_get_allocation_count());
    
    // Test gc_strdup
    char *ptr2 = gc_strdup("Hello, World!");
    printf("After gc_strdup: %zu allocations\n", gc_get_allocation_count());
    
    // Test gc_calloc
    int *ptr3 = gc_calloc(10, sizeof(int));
    printf("After gc_calloc(10 ints): %zu allocations\n", gc_get_allocation_count());
    
    // Use the allocated memory
    if (ptr1 && ptr2 && ptr3)
    {
        ft_strlcpy(ptr1, ptr2, 100);
        printf("String copied: %s\n", ptr1);
        
        for (int i = 0; i < 10; i++)
            ptr3[i] = i * i;
        printf("Array filled with squares: ");
        for (int i = 0; i < 10; i++)
            printf("%d ", ptr3[i]);
        printf("\n");
    }
    
    // Test partial cleanup (simulating command completion)
    printf("Before partial cleanup: %zu allocations\n", gc_get_allocation_count());
    gc_cleanup_partial();
    printf("After partial cleanup: %zu allocations\n", gc_get_allocation_count());
    
    // Allocate more memory
    char *ptr4 = gc_strdup("After cleanup");
    printf("After new allocation: %zu allocations\n", gc_get_allocation_count());
    printf("New string: %s\n", ptr4);
    
    // Final cleanup
    printf("Before final cleanup: %zu allocations\n", gc_get_allocation_count());
    gc_cleanup();
    printf("After final cleanup: %zu allocations\n", gc_get_allocation_count());
    
    printf("=== Test Complete ===\n");
}

int main(void)
{
    test_garbage_collector();
    return (0);
}
