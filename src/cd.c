/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 14:16:43 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/12 21:29:36 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	update_oldpwd(char ***env)
{
	int		i;
	char	*oldpwd;
	char	*new_oldpwd;

	oldpwd = get_env("PWD", *env);
	if (!oldpwd)
		return ;
	new_oldpwd = ft_strjoin("OLDPWD=", oldpwd);
	if (!new_oldpwd)
		return ;
	i = 0;
	while ((*env)[i])
	{
		if (ft_strncmp((*env)[i], "OLDPWD=", 7) == 0)
		{
			free((*env)[i]);
			(*env)[i] = new_oldpwd;
			return ;
		}
		i++;
	}
	add_env_variable(new_oldpwd, env);
}

static void	handle_getcwd_error(void)
{
	write(2, "cd: error retrieving current directory: getcwd: cannot ", 56);
	write(2, "access parent directories: No such file or directory\n", 54);
	set_status(1);
}

void	update_pwd(char ***env)
{
	int		i;
	char	*cwd;
	char	*new_pwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		handle_getcwd_error();
		return ;
	}
	update_saved_cwd_public();
	new_pwd = ft_strjoin("PWD=", cwd);
	if (free(cwd), !new_pwd)
		return ;
	i = -1;
	while ((*env)[++i])
		if (strncmp((*env)[i], "PWD=", 4) == 0)
			return (free((*env)[i]), (void)((*env)[i] = new_pwd));
	add_env_variable(new_pwd, env);
}

static char	*get_cd_target(char *cmd, char **env)
{
	char	*target;

	if (!cmd)
	{
		target = get_env("HOME", env);
		if (!target)
		{
			write(2, "cd: HOME not set\n", 18);
			set_status(1);
			return (NULL);
		}
	}
	else
		target = cmd;
	return (target);
}

void	cd(char *cmd, char ***env)
{
	char	*target;

	target = get_cd_target(cmd, *env);
	if (!target)
		return ;
	if (chdir(target) == -1)
	{
		set_status(1);
		perror(target);
		return ;
	}
	update_oldpwd(env);
	update_pwd(env);
	set_status(0);
}
