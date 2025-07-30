/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dup_parent_and_child.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 17:19:17 by helfatih          #+#    #+#             */
/*   Updated: 2025/07/30 17:36:37 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	open_and_duplicate(t_command **cmd, int *flags, int *fd_out)
{
	if ((*cmd)->file_output)
	{
		*flags = O_WRONLY | O_CREAT | append_or_trunc(cmd);
		*fd_out = open((*cmd)->file_output, *flags, 0644);
		if (*fd_out < 0)
			return ;
		if (dup2(*fd_out, STDOUT_FILENO) == -1)
		{
			close(*fd_out);
			return ;
		}
		close(*fd_out);
	}
}

int	is_directory_parent(t_command **cmd)
{
	DIR	*folder;

	folder = opendir((*cmd)->file_output);
	if (folder != NULL)
	{
		printf("minishell : %s:  Is a directory\n", (*cmd)->file_output);
		set_status(1);
		closedir(folder);
		return (1);
	}
	closedir(folder);
	return (0);
}

void	excute_redirection_of_parent(t_command **cmd, int *fd_out, t_data *data, int *fd1, char **env)
{
	int(saved_stdout), saved_stdin, flags;
	int error = 0;
	if ((*cmd)->file_output)
	{
		if (is_directory_parent(cmd))
			return ;
	}
	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	open_and_duplicate(cmd, &flags, fd_out);
	my_exit(cmd, data, &error);
	if (error == 1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdout);
		close(saved_stdin);
		close(*fd1);
		gc_cleanup();
		rl_clear_history();
		exit(get_status());
	}
	execute_builtin_command(*cmd, env);
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin);
}

void	excute_redirection_of_child_builtin(t_command **cmd, int *fd_out,
		t_data *data, int *fd1, int *fd2, char **env)
{
	(void)fd2;
	int(saved_stdout), saved_stdin, flags;
	int	error = 0;
	if ((*cmd)->file_output)
	{
		if (is_directory_parent(cmd))
			return ;
	}
	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	open_and_duplicate(cmd, &flags, fd_out);
	my_exit_child(cmd, data, &error);
	if (error == 1)
	{
		close(saved_stdin);
		close(saved_stdout);
		close(*fd1);
		close(*fd2);
		gc_cleanup();
		rl_clear_history();
		exit(get_status());
	}
	execute_builtin_command(*cmd, env);
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin);
}
