/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/07 17:35:33 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	*create_new_entry(char *name, char *value)
{
	char	*new_entry;
	char	*tmp;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (NULL);
	new_entry = ft_strjoin(tmp, value);
	free(tmp);
	return (new_entry);
}

static int	find_and_update_var(char **env, char *name, char *new_entry,
		size_t len)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
		{
			free(env[i]);
			env[i] = new_entry;
			return (1);
		}
		i++;
	}
	return (0);
}

int	update_existing_var(char *name, char *value, char **env)
{
	size_t	len;
	char	*new_entry;

	len = ft_strlen(name);
	new_entry = create_new_entry(name, value);
	if (!new_entry)
		return (0);
	if (find_and_update_var(env, name, new_entry, len))
		return (1);
	free(new_entry);
	return (0);
}
