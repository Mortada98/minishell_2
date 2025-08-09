/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_unset.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/09 12:15:51 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	handle_invalid_identifier_error(char *arg)
{
	write(2, "minishell: export: `", 20);
	write(2, arg, ft_strlen(arg));
	write(2, "': not a valid identifier\n", 26);
	set_status(1);
	return (0);
}

int	my_export(char *arg, char ***env)
{
	char	*equal_sign;

	if (!arg || !*arg)
	{
		print_export_env(*env);
		return (1);
	}
	equal_sign = ft_strchr(arg, '=');
	if (!equal_sign)
	{
		if (!is_valid_identifier(arg))
			return (handle_invalid_identifier_error(arg));
		return (export_without_value(arg, env));
	}
	return (export_with_value(arg, equal_sign, env));
}

static char	**allocate_new_env(char **env)
{
	int		count;
	char	**new_env;

	count = 0;
	while (env[count])
		count++;
	new_env = malloc(sizeof(char *) * (count + 1));
	return (new_env);
}

static void	copy_env_without_var(char **env, char **new_env, char *name,
		size_t len)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (env[i])
	{
		if (!(ft_strncmp(env[i], name, len) == 0 && env[i][len] == '='))
			new_env[j++] = env[i];
		else
			free(env[i]);
		i++;
	}
	new_env[j] = NULL;
}

int	my_unset(char *name, char ***env)
{
	size_t	len;
	char	**new_env;

	len = ft_strlen(name);
	new_env = allocate_new_env(*env);
	if (!new_env)
		return (0);
	copy_env_without_var(*env, new_env, name, len);
	free(*env);
	*env = new_env;
	return (1);
}
