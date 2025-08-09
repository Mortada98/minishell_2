/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_without_value.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/08 21:07:14 by helfatih         ###   ########.fr       */
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

static char	*create_empty_var(char *name)
{
	char	*new_var;
	char	*tmp;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (NULL);
	new_var = ft_strjoin(tmp, "");
	free(tmp);
	return (new_var);
}

int	export_without_value(char *name, char ***env)
{
	size_t	len;
	char	*new_var;

	len = ft_strlen(name);
	if (var_already_exists(name, *env, len))
		return (1);
	new_var = create_empty_var(name);
	if (!new_var)
		return (0);
	add_env_variable(new_var, env);
	return (1);
}
