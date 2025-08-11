/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 18:47:29 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	*manage_saved_cwd(int action, char *new_value)
{
	static char	*saved_cwd = NULL;
	char		*temp;

	if (action == 0) // GET
	{
		if (saved_cwd)
			return (ft_strdup(saved_cwd));
		return (NULL);
	}
	else if (action == 1) // SET
	{
		if (saved_cwd)
			free(saved_cwd);
		saved_cwd = new_value;
		return (NULL);
	}
	else if (action == 2) // UPDATE from getcwd
	{
		temp = getcwd(NULL, 0);
		if (temp)
		{
			if (saved_cwd)
				free(saved_cwd);
			saved_cwd = temp;
		}
		return (NULL);
	}
	else if (action == 3) // CLEANUP
	{
		if (saved_cwd)
		{
			free(saved_cwd);
			saved_cwd = NULL;
		}
		return (NULL);
	}
	return (NULL);
}

static char	*get_current_directory(char **env)
{
	char	*pwd_env;
	char	*cwd;
	char	*saved_cwd;

	// Update saved cwd if possible
	manage_saved_cwd(2, NULL);
	
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
	saved_cwd = manage_saved_cwd(0, NULL);
	if (saved_cwd)
		return (saved_cwd);
	
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
	manage_saved_cwd(2, NULL);
}

void	update_saved_cwd_public(void)
{
	manage_saved_cwd(2, NULL);
}

char	*get_saved_cwd(void)
{
	return (manage_saved_cwd(0, NULL));
}

void	cleanup_saved_cwd(void)
{
	manage_saved_cwd(3, NULL);
}
