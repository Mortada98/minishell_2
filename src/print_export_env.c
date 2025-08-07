/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_export_env.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/07 17:35:33 by mbouizak         ###   ########.fr       */
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

void	print_export_env(char **env)
{
	int		count;
	char	**sorted_env;
	int		i;

	count = 0;
	while (env[count])
		count++;
	sorted_env = copy_and_sort_env(env, count);
	if (!sorted_env)
		return ;
	i = 0;
	while (i < count)
	{
		print_env_variable(sorted_env[i]);
		i++;
	}
	free_sorted_env(sorted_env, count);
}
