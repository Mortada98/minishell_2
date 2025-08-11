/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:48:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 17:14:03 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	handle_single_builtin(t_command *cmd, char ***env, t_data **data, t_fd *fd)
{
	fd->fd_out = -1;
	excute_redirection_of_parent(&cmd, fd, *data, env);
	dup2(fd->saved_stdin, 0);
	close(fd->saved_stdin);
	return (0);
}

static int	check_redirections(t_command *cmd)
{
	t_command	*temp_cmd;

	temp_cmd = cmd;
	while (temp_cmd)
	{
		if (temp_cmd->redir || temp_cmd->herdoc)
			return (1);
		temp_cmd = temp_cmd->next;
	}
	return (0);
}

static void	execute_redirection_child(t_command *cmd, t_data **data,
		int saved_stdin, char ***env)
{
	t_exec_params	params;

	excute_redirection_of_child(&cmd, data, &params, *env);
	if ((*env)[0])
		free_2d_array(*env);
	close(saved_stdin);
	close_fds_except_std();
	gc_cleanup();
	exit(0);
}

static int	handle_redirection_process(t_command *cmd, t_data **data,
		int saved_stdin, char ***env)
{
	pid_t	redir_pid;
	int		status;

	redir_pid = fork();
	if (redir_pid == 0)
		execute_redirection_child(cmd, data, saved_stdin, env);
	else if (redir_pid > 0)
	{
		waitpid(redir_pid, &status, 0);
		if (WIFEXITED(status))
			set_status(WEXITSTATUS(status));
	}
	return (0);
}

int	handle_empty_command(t_command *cmd, t_data **data, int saved_stdin,
		char ***env)
{
	if (!check_redirections(cmd))
	{
		dup2(saved_stdin, 0);
		close(saved_stdin);
		return (0);
	}
	handle_redirection_process(cmd, data, saved_stdin, env);
	dup2(saved_stdin, 0);
	close(saved_stdin);
	return (0);
}
