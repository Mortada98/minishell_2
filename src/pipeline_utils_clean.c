/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils_clean.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 22:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/12 20:29:47 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	initialize_pipeline_state(t_exec_params *params,
		t_pipeline_state *state)
{
	state->prev_fd = -1;
	state->pid_count = 0;
	state->fd_in = -1;
	state->fd_out = -1;
	params->prev_fd = state->prev_fd;
	params->fd_out = &state->fd_out;
	params->fd_in = &state->fd_in;
}

int	process_pipeline_helper(int pid, t_parent_params *parent_params,
		t_child_params *child_params, t_exec_params *params)
{
	if (pid == 0)
		handle_child_fork(child_params, params);
	else if (pid > 0)
		handle_parent_process(pid, child_params->curr, parent_params);
	else
	{
		handle_fork_error(params->save, params->saved_stdin);
		return (0);
	}
	if (params->save >= 0)
	{
		if (!child_params->curr->next)
		{
			close(parent_params->fd[0]);
			close(parent_params->fd[1]);
		}
		dup2(params->save, 0);
		close(params->save);
	}
	return (1);
}

int	process_pipeline_iteration(t_child_params *child_params,
		t_exec_params *params, t_parent_params *parent_params)
{
	pid_t	pid;

	params->prev_fd = *(parent_params->prev_fd);
	params->save = dup(0);
	if (params->save < 0)
		return (0);
	if (!setup_pipe(child_params->curr, params->fd))
	{
		close(params->save);
		dup2(params->saved_stdin, 0);
		close(params->saved_stdin);
		return (0);
	}
	pid = fork();
	if (process_pipeline_helper(pid, parent_params, child_params, params) == 0)
		return (0);
	return (1);
}

int	setup_params_helper(t_child_params *child_params, t_exec_params *params,
		t_pipeline_state state, t_parent_params parent_params)
{
	t_command	*curr;

	curr = child_params->cmd;
	while (curr && !curr->file)
	{
		child_params->curr = curr;
		if (!process_pipeline_iteration(child_params, params, &parent_params))
		{
			cleanup_pipeline_fds(params, &state);
			dup2(params->saved_stdin, 0);
			close(params->saved_stdin);
			return (0);
		}
		curr = curr->next;
	}
	return (1);
}

void	execute_pipeline_loop(t_command *cmd, t_data **data, char ***env,
		int saved_stdin)
{
	t_exec_params	params;

	params.saved_stdin = saved_stdin;
	setup_params_and_loop(cmd, data, env, &params);
}
