/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 10:28:49 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	handle_export_error(char *arg, char *name)
{
	(void)arg;
	write(2, "minishell: export: not a valid identifier\n", 43);
	if (name)
		free(name);
	set_status(1);
	return (0);
}

static int	add_new_variable(char *name, char *value, char ***env)
{
	char	*tmp;
	char	*new_var;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (0);
	new_var = ft_strjoin(tmp, value);
	free(tmp);
	if (!new_var)
		return (0);
	add_env_variable(new_var, env);
	return (1);
}

static int	process_export_value(char *name, char *value, char ***env)
{
	remove_exported_var(name);
	if (!update_existing_var(name, value, *env))
	{
		if (!add_new_variable(name, value, env))
		{
			free(name);
			free(value);
			return (0);
		}
	}
	free(name);
	free(value);
	set_status(0);
	return (1);
}

int	export_with_value(char *arg, char *equal_sign, char ***env)
{
	char	*name;
	char	*value;

	name = ft_substr(arg, 0, equal_sign - arg);
	if (!name)
		return (0);
	if (!is_valid_identifier(name))
		return (handle_export_error(arg, name));
	value = ft_strdup(equal_sign + 1);
	if (!value)
	{
		free(name);
		return (0);
	}
	return (process_export_value(name, value, env));
}
