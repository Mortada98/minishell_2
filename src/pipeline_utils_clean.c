/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils_clean.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 22:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/07 18:20:37 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	initialize_pipeline_state(t_exec_params *params,
	t_pipeline_state *state)
{
	state->prev_fd = -1;
	state->pid_count = 0;
	params->prev_fd = state->prev_fd;
	params->fd_out = &state->fd_out;
	params->fd_in = &state->fd_in;
}

static void	handle_child_fork(t_child_params *child_params,
	t_exec_params *params)
{
	if ((child_params->cmd)->file)
	{
		(child_params->cmd)->file = false;
		set_status(130);
		exit(130);
	}
	execute_child_process(child_params->curr, child_params->data, params,
		child_params->env);
}

static int	process_pipeline_iteration(t_child_params *child_params,
	t_exec_params *params, t_parent_params *parent_params)
{
	pid_t	pid;

	params->save = dup(0);
	if (!setup_pipe(child_params->curr, params->fd))
	{
		close(params->save);
		dup2(params->saved_stdin, 0);
		close(params->saved_stdin);
		return (0);
	}
	pid = fork();
	if (pid == 0)
		handle_child_fork(child_params, params);
	else if (pid > 0)
		handle_parent_process(pid, child_params->curr, parent_params);
	else
	{
		handle_fork_error(params->save, params->saved_stdin);
		return (0);
	}
	dup2(params->save, 0);
	close(params->save);
	return (1);
}

void	execute_pipeline_loop(t_command *cmd, t_data **data, char ***env,
	int saved_stdin)
{
	t_command			*curr;
	t_exec_params		params;
	t_parent_params		parent_params;
	t_child_params		child_params;
	t_pipeline_state	state;

	curr = cmd;
	initialize_pipeline_state(&params, &state);
	params.saved_stdin = saved_stdin;
	params.fd = (int [2]){0, 0};
	parent_params.prev_fd = &state.prev_fd;
	parent_params.fd = params.fd;
	parent_params.pids = (pid_t [1024]){0};
	parent_params.pid_count = &state.pid_count;
	child_params.cmd = cmd;
	child_params.data = data;
	child_params.env = env;
	while (curr && !(cmd)->file)
	{
		child_params.curr = curr;
		if (!process_pipeline_iteration(&child_params, &params,
				&parent_params))
			return ;
		curr = curr->next;
	}
	handle_core_dumped(parent_params.pids, state.pid_count, data);
	signal(SIGINT, my_handler);
}
