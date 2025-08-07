/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_sort_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/07 17:35:33 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	free_sorted_env(char **sorted_env, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		free(sorted_env[i]);
		i++;
	}
	free(sorted_env);
}

static char	**allocate_and_copy_env(char **env, int count)
{
	char	**sorted_env;
	int		i;

	sorted_env = malloc(sizeof(char *) * (count + 1));
	if (!sorted_env)
		return (NULL);
	i = 0;
	while (i < count)
	{
		sorted_env[i] = ft_strdup(env[i]);
		if (!sorted_env[i])
			return (free_sorted_env(sorted_env, i), NULL);
		i++;
	}
	sorted_env[count] = NULL;
	return (sorted_env);
}

static void	sort_env_array(char **sorted_env, int count)
{
	char	*temp;
	int		i;
	int		j;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strcmp(sorted_env[j], sorted_env[j + 1]) > 0)
			{
				temp = sorted_env[j];
				sorted_env[j] = sorted_env[j + 1];
				sorted_env[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

char	**copy_and_sort_env(char **env, int count)
{
	char	**sorted_env;

	sorted_env = allocate_and_copy_env(env, count);
	if (!sorted_env)
		return (NULL);
	sort_env_array(sorted_env, count);
	return (sorted_env);
}
