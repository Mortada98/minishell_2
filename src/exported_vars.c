/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exported_vars.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 18:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/12 22:38:51 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	add_exported_var(char *name)
{
	t_exported_var	*new_var;
	t_exported_var	*current;
	t_exported_var	**exported_vars;

	exported_vars = get_exported_vars_ptr();
	if (is_exported_var(name))
		return ;
	new_var = gc_malloc(sizeof(t_exported_var));
	if (!new_var)
		return ;
	new_var->name = gc_strdup(name);
	if (!new_var->name)
		return ;
	new_var->next = NULL;
	if (!*exported_vars)
	{
		*exported_vars = new_var;
		return ;
	}
	current = *exported_vars;
	while (current->next)
		current = current->next;
	current->next = new_var;
}

void	remove_exported_var(char *name)
{
	t_exported_var	*current;
	t_exported_var	*prev;
	t_exported_var	**exported_vars;

	exported_vars = get_exported_vars_ptr();
	if (!*exported_vars)
		return ;
	current = *exported_vars;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->name, name) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*exported_vars = current->next;
			return ;
		}
		prev = current;
		current = current->next;
	}
}

int	is_exported_var(char *name)
{
	t_exported_var	*current;
	t_exported_var	**exported_vars;

	exported_vars = get_exported_vars_ptr();
	current = *exported_vars;
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
	t_exported_var	**exported_vars;

	exported_vars = get_exported_vars_ptr();
	*exported_vars = NULL;
}

t_exported_var	*get_exported_vars(void)
{
	t_exported_var	**exported_vars;

	exported_vars = get_exported_vars_ptr();
	return (*exported_vars);
}
