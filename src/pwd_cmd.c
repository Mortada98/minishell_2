/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:34:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 10:23:16 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	cleanup_and_exit2(int save, int saved_stdin, char ***env, int status)
{
	(void)status;
	dup2(save, 0);
	close(save);
	close(saved_stdin);
	close_fds_except_std();
	gc_cleanup();
	if ((*env)[0])
		free_2d_array(*env);
	rl_clear_history();
	exit(get_status());
}

char	*manage_saved_cwd(int action, char *new_value)
{
	static char	*saved_cwd = NULL;
	char		*temp;

	if (action == 0)
	{
		if (saved_cwd)
			return (gc_strdup(saved_cwd));
		return (NULL);
	}
	else if (action == 1)
	{
		saved_cwd = new_value;
		return (NULL);
	}
	else if (action == 2)
	{
		temp = getcwd(NULL, 0);
		gc_register_external(temp);
		if (temp)
			saved_cwd = temp;
		return (NULL);
	}
	saved_cwd = NULL;
	return (NULL);
}

char	*get_current_directory(char **env)
{
	char	*pwd_env;
	char	*cwd;
	char	*saved_cwd;

	manage_saved_cwd(2, NULL);
	pwd_env = get_env("PWD", env);
	if (pwd_env)
	{
		cwd = getcwd(NULL, 0);
		gc_register_external(cwd);
		if (cwd)
			return (gc_strdup(pwd_env));
		return (gc_strdup(pwd_env));
	}
	cwd = getcwd(NULL, 0);
	gc_register_external(cwd);
	if (cwd)
		return (cwd);
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
		set_status(0);
	}
	else
	{
		ft_putstr_fd("pwd: error retrieving current directory: ",
			STDERR_FILENO);
		ft_putstr_fd("getcwd: cannot access parent directories: ",
			STDERR_FILENO);
		ft_putendl_fd("No such file or directory", STDERR_FILENO);
		set_status(1);
	}
}
