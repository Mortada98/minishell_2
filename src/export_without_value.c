/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_without_value.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 18:19:09 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	var_already_exists(char *name, char **env, size_t len)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, len) == 0 && (env[i][len] == '='
			|| env[i][len] == '\0'))
			return (1);
		i++;
	}
	return (0);
}

int	export_without_value(char *name, char ***env)
{
	size_t	len;

	len = ft_strlen(name);
	
	// Check if variable already exists in environment
	if (var_already_exists(name, *env, len))
		return (1);
	
	// Add to exported variables list (without value)
	add_exported_var(name);
	return (1);
}
