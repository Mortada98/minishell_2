/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dup_parent_and_child.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 17:19:17 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 17:13:39 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	open_and_duplicate(t_command **cmd, int *flags, int *fd_out)
{
	t_redir	*temp;
	int		fd_in;

	if (!(*cmd)->redir)
		return (1);
	temp = (*cmd)->redir;
	fd_in = -1;
	while (temp)
	{
		if (temp->type == TOKEN_REDIR_IN)
		{
			if (fd_in > 2)
				close(fd_in);
			fd_in = open(temp->data, O_RDONLY);
			if (fd_in < 0)
			{
				write(2, "minishell: No such file or directory\n", 38);
				set_status(1);
				return (0);
			}
			if (dup2(fd_in, STDIN_FILENO) == -1)
			{
				perror("minishell");
				close(fd_in);
				return (0);
			}
			close(fd_in);
		}
		else if (temp->type == TOKEN_REDIR_OUT)
		{
			*flags = O_WRONLY | O_CREAT | O_TRUNC;
			*fd_out = open(temp->data, *flags, 0644);
			if (*fd_out < 0)
			{
				write(2, "minishell: Permission denied\n", 30);
				set_status(1);
				return (0);
			}
			if (dup2(*fd_out, STDOUT_FILENO) == -1)
			{
				perror("minishell");
				close(*fd_out);
				return (0);
			}
			close(*fd_out);
		}
		else if (temp->type == TOKEN_REDIR_APPEND)
		{
			*flags = O_WRONLY | O_CREAT | O_APPEND;
			*fd_out = open(temp->data, *flags, 0644);
			if (*fd_out < 0)
			{
				write(2, "minishell: Permission denied\n", 30);
				set_status(1);
				return (0);
			}
			if (dup2(*fd_out, STDOUT_FILENO) == -1)
			{
				perror("minishell");
				close(*fd_out);
				return (0);
			}
			close(*fd_out);
		}
		temp = temp->next;
	}
	return (1);
}

int	is_directory_parent(t_command **cmd)
{
	int		fd;
	t_redir	*temp;

	if (!(*cmd)->redir)
		return (0);
	temp = (*cmd)->redir;
	while (temp)
	{
		fd = open(temp->data, O_WRONLY | O_CREAT, 0644);
		if (fd < 0)
		{
			print_errno(temp);
			return (1);
		}
		close(fd);
		temp = temp->next;
	}
	return (0);
}

void	excute_redirection_of_parent(t_command **cmd, t_fd *fd, t_data *data,
		char ***env)
{
	int	error;
	int saved_stdout1;
	int	saved_stdin1;
	int	flags;
	
	error = 0;
	saved_stdout1 = dup(STDOUT_FILENO);
	saved_stdin1 = dup(STDIN_FILENO);
	if (!open_and_duplicate(cmd, &flags, &fd->fd_out))
	{
		dup2(saved_stdout1, STDOUT_FILENO);
		dup2(saved_stdin1, STDIN_FILENO);
		close(saved_stdout1);
		close(saved_stdin1);
		return ;
	}
	my_exit(cmd, data, &error);
	if (error == 1)
		close_fd2(&saved_stdin1, &saved_stdout1, fd, env);
	execute_builtin_command(*cmd, env);
	dup2(saved_stdout1, STDOUT_FILENO);
	dup2(saved_stdin1, STDIN_FILENO);
	close(saved_stdout1);
	close(saved_stdin1);
}

void	close_fd(int *saved_stdin1, int *saved_stdout, t_builtin_params *param)
{
	if (*param->save >= 0)
	{
		dup2(*param->save, 0);
		close(*param->save);
	}
	if (*saved_stdin1 >= 0)
		close(*saved_stdin1);
	if (*saved_stdout >= 0)
		close(*saved_stdout);
	if (*param->fd_out >= 0)
		close(*param->fd_out);
	if (*param->saved_stdin >= 0)
		close(*param->saved_stdin);
	gc_cleanup();
	free_2d_array(*param->env);
	rl_clear_history();
	exit(get_status());
}

void	excute_redirection_of_child_builtin(t_builtin_params *param)
{
	int		error;
	int		saved_stdout;
	int		saved_stdin1;
	int		flags;
	int		fd_in;

	error = 0;
	if ((*param->curr)->redir)
		if (is_directory_parent(param->curr))
			return ;
	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin1 = dup(STDIN_FILENO);
	if (execute_red_child_check(param, &fd_in))
		return ;
	open_and_duplicate(param->curr, &flags, param->fd_out);
	my_exit_child(param->curr, param->data, &error);
	if (error == 1 || get_status() == 1)
		close_fd(&saved_stdin1, &saved_stdout, param);
	execute_builtin_command((*param->curr), param->env);
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin1, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin1);
}
