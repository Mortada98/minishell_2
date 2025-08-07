/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:48:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/07 19:02:34 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	handle_single_builtin(t_command *cmd, char ***env, t_data **data,
	int saved_stdin)
{
	int	fd_out;

	fd_out = -1;
	if (!access_file(cmd))
		return (close(saved_stdin), 0);
	if (cmd->redir_error)
	{
		dup2(saved_stdin, 0);
		close(saved_stdin);
		set_status(1);
		return (0);
	}
	excute_redirection_of_parent(&cmd, &fd_out, *data, &saved_stdin, env);
	dup2(saved_stdin, 0);
	close(saved_stdin);
	return (0);
}

static int	check_redirections(t_command *cmd)
{
	t_command	*temp_cmd;

	temp_cmd = cmd;
	while (temp_cmd)
	{
		if (temp_cmd->file_input || temp_cmd->file_output || temp_cmd->herdoc)
			return (1);
		temp_cmd = temp_cmd->next;
	}
	return (0);
}

static void	execute_redirection_child(t_command *cmd, t_data **data,
	int saved_stdin, char ***env)
{
	int	fd_out;
	int	fd_in;

	excute_redirection_of_child(&cmd, data, &fd_out, &fd_in);
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
