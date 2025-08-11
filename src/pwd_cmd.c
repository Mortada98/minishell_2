/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 17:58:47 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	*g_saved_cwd = NULL;

static void	update_saved_cwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		if (g_saved_cwd)
			free(g_saved_cwd);
		g_saved_cwd = cwd;
	}
}

static char	*get_current_directory(char **env)
{
	char	*pwd_env;
	char	*cwd;

	// Update saved cwd if possible
	update_saved_cwd();
	
	// First try to get PWD from environment (logical path)
	pwd_env = get_env("PWD", env);
	if (pwd_env)
	{
		// Verify if the PWD path still exists
		cwd = getcwd(NULL, 0);
		if (cwd)
		{
			free(cwd);
			return (ft_strdup(pwd_env));
		}
		// If getcwd fails but PWD exists, still return PWD (bash behavior)
		return (ft_strdup(pwd_env));
	}
	
	// If PWD is not set, try getcwd as fallback
	cwd = getcwd(NULL, 0);
	if (cwd)
		return (cwd);
	
	// If both fail, use saved cwd as last resort
	if (g_saved_cwd)
		return (ft_strdup(g_saved_cwd));
	
	return (NULL);
}

void	my_pwd(char **env)
{
	char	*path;

	path = get_current_directory(env);
	if (path)
	{
		printf("%s\n", path);
		free(path);
		set_status(0);
	}
	else
	{
		// When both getcwd() fails and PWD is unset
		ft_putstr_fd("pwd: error retrieving current directory: ", STDERR_FILENO);
		ft_putstr_fd("getcwd: cannot access parent directories: ", STDERR_FILENO);
		ft_putendl_fd("No such file or directory", STDERR_FILENO);
		set_status(1);
	}
}

void	init_saved_cwd(void)
{
	update_saved_cwd();
}

void	update_saved_cwd_public(void)
{
	update_saved_cwd();
}

char	*get_saved_cwd(void)
{
	if (g_saved_cwd)
		return (ft_strdup(g_saved_cwd));
	return (NULL);
}

void	cleanup_saved_cwd(void)
{
	if (g_saved_cwd)
	{
		free(g_saved_cwd);
		g_saved_cwd = NULL;
	}
}
