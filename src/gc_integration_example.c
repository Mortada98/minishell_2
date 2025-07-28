/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_integration_example.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 10:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/07/28 10:00:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/*
 * Example: Modified token creation function using garbage collector
 * This replaces the original creat_token function
 */
t_token *gc_creat_token(char *line, t_token_type type, bool should_join)
{
	t_token *new_token;

	new_token = (t_token *)gc_calloc(1, sizeof(t_token));
	if (!new_token)
		return (NULL);
	
	new_token->av = gc_strdup(line);
	if (!new_token->av)
		return (NULL);
	
	new_token->type = type;
	new_token->info = should_join;
	new_token->next = NULL;
	new_token->prev = NULL;
	return (new_token);
}

/*
 * Example: Modified command creation function using garbage collector
 * This replaces the original create_command function
 */
t_command *gc_create_command(void)
{
	t_command *cmd;

	cmd = (t_command *)gc_calloc(1, sizeof(t_command));
	if (!cmd)
		return (NULL);
	
	cmd->args = NULL;
	cmd->file_input = NULL;
	cmd->file_output = NULL;
	cmd->herdoc = NULL;
	cmd->herdoc_file = NULL;
	cmd->file = false;
	cmd->next = NULL;
	return (cmd);
}

/*
 * Example: Modified append_arg function using garbage collector
 * This replaces the original append_arg function
 */
void gc_append_arg(t_command *cmd, char *str, t_data **data)
{
	int i, j;
	char **new_array;
	
	(*data)->exit = 0;
	i = 0;
	if (cmd->args)
		i = count_args(cmd->args);
	
	new_array = gc_calloc(i + 2, sizeof(char *));
	if (!new_array)
		return;
	
	j = -1;
	while (++j < i)
		new_array[j] = cmd->args[j];
	
	new_array[i] = gc_strdup(str);
	if (!new_array[i])
	{
		gc_free(new_array);
		return;
	}
	
	new_array[i + 1] = NULL;
	
	// Don't free cmd->args if it was allocated with gc_malloc
	// The garbage collector will handle it
	cmd->args = new_array;
}

/*
 * Example: Modified expand_env function using garbage collector
 * This shows how to replace malloc/calloc calls with gc equivalents
 */
char *gc_expand_env(char *str)
{
	char *result;
	size_t old_size;
	
	if (!str)
		return (NULL);
	
	old_size = ft_strlen(str) + 100;
	result = (char *)gc_calloc(old_size, 1);
	if (!result)
		return (NULL);
	
	// ... rest of the expansion logic using gc_malloc for any additional allocations
	// This is just a simplified example
	ft_strlcpy(result, str, old_size);
	
	return (result);
}

/*
 * Example: How to use the garbage collector in main function
 */
void example_main_integration(void)
{
	char *line;
	t_token *tokens;
	t_command *commands;
	t_data *data;
	
	// Initialize data structure with garbage collector
	data = gc_calloc(1, sizeof(t_data));
	
	while (1)
	{
		line = readline("minishell> ");
		if (!line)
			break;
		
		if (*line)
			add_history(line);
		
		// Tokenize using garbage collector
		tokens = tokenize(line, &data);
		
		// Parse commands using garbage collector
		commands = parsing_command(tokens, &data);
		
		// Execute commands
		execute_command(commands, environ, &data);
		
		// Clean up allocations for this iteration
		// This frees all memory allocated during this loop iteration
		gc_cleanup_partial();
		
		free(line); // readline allocates with malloc, not our gc
	}
	
	// Final cleanup - this will free any remaining allocations
	gc_cleanup();
}

/*
 * Signal handler that ensures cleanup on exit
 */
void gc_signal_handler(int sig)
{
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		gc_cleanup();
		exit(130);
	}
}

/*
 * Example: Modified main function with garbage collector integration
 */
int gc_main_example(int argc, char **argv, char **env)
{
	(void)argc;
	(void)argv;
	(void)env;
	
	// Set up signal handling with cleanup
	signal(SIGINT, gc_signal_handler);
	signal(SIGQUIT, SIG_IGN);
	
	// Your main shell loop here
	example_main_integration();
	
	// Cleanup before exit
	gc_cleanup();
	
	return (0);
}
