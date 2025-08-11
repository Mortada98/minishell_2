/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_export_env.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 18:19:09 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	print_env_variable(char *env_var)
{
	char	*equal_pos;

	equal_pos = ft_strchr(env_var, '=');
	if (equal_pos)
	{
		*equal_pos = '\0';
		printf("declare -x %s=\"%s\"\n", env_var, equal_pos + 1);
		*equal_pos = '=';
	}
	else
	{
		printf("declare -x %s\n", env_var);
	}
}

static int	count_total_vars(char **env, t_exported_var *exported)
{
	int				count;
	t_exported_var	*current;

	count = 0;
	while (env[count])
		count++;
	
	current = exported;
	while (current)
	{
		count++;
		current = current->next;
	}
	
	return (count);
}

static char	**create_combined_list(char **env, t_exported_var *exported, int total_count)
{
	char			**combined;
	int				i;
	t_exported_var	*current;

	combined = malloc(sizeof(char *) * (total_count + 1));
	if (!combined)
		return (NULL);
	
	// Copy environment variables
	i = 0;
	while (env[i])
	{
		combined[i] = ft_strdup(env[i]);
		if (!combined[i])
			return (free_sorted_env(combined, i), NULL);
		i++;
	}
	
	// Add exported variables without values
	current = exported;
	while (current)
	{
		combined[i] = ft_strdup(current->name);
		if (!combined[i])
			return (free_sorted_env(combined, i), NULL);
		i++;
		current = current->next;
	}
	
	combined[total_count] = NULL;
	return (combined);
}

void	print_export_env(char **env)
{
	int				total_count;
	char			**combined_list;
	t_exported_var	*exported;
	int				i;

	exported = get_exported_vars();
	total_count = count_total_vars(env, exported);
	
	combined_list = create_combined_list(env, exported, total_count);
	if (!combined_list)
		return ;
	
	// Sort the combined list
	sort_env_array(combined_list, total_count);
	
	// Print all variables
	i = 0;
	while (i < total_count)
	{
		print_env_variable(combined_list[i]);
		i++;
	}
	
	free_sorted_env(combined_list, total_count);
}
