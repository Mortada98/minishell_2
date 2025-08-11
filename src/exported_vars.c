/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exported_vars.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 18:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/11 18:00:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static t_exported_var *g_exported_vars = NULL;

void	add_exported_var(char *name)
{
	t_exported_var	*new_var;
	t_exported_var	*current;

	// Check if already exists
	if (is_exported_var(name))
		return ;
	
	new_var = malloc(sizeof(t_exported_var));
	if (!new_var)
		return ;
	
	new_var->name = ft_strdup(name);
	if (!new_var->name)
	{
		free(new_var);
		return ;
	}
	
	new_var->next = NULL;
	
	if (!g_exported_vars)
	{
		g_exported_vars = new_var;
		return ;
	}
	
	current = g_exported_vars;
	while (current->next)
		current = current->next;
	current->next = new_var;
}

void	remove_exported_var(char *name)
{
	t_exported_var	*current;
	t_exported_var	*prev;

	if (!g_exported_vars)
		return ;
	
	current = g_exported_vars;
	prev = NULL;
	
	while (current)
	{
		if (ft_strcmp(current->name, name) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				g_exported_vars = current->next;
			
			free(current->name);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

int	is_exported_var(char *name)
{
	t_exported_var	*current;

	current = g_exported_vars;
	while (current)
	{
		if (ft_strcmp(current->name, name) == 0)
			return (1);
		current = current->next;
	}
	return (0);
}

void	cleanup_exported_vars(void)
{
	t_exported_var	*current;
	t_exported_var	*next;

	current = g_exported_vars;
	while (current)
	{
		next = current->next;
		free(current->name);
		free(current);
		current = next;
	}
	g_exported_vars = NULL;
}

t_exported_var	*get_exported_vars(void)
{
	return (g_exported_vars);
}
