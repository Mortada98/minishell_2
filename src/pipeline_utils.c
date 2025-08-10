/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 18:13:22 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/10 18:30:54 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	handle_child_fork(t_child_params *child_params, t_exec_params *params)
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

void	cleanup_pipeline_fds(t_exec_params *params, t_pipeline_state *state)
{
	if (state->fd_in > 2)
		close(state->fd_in);
	if (state->fd_out > 2)
		close(state->fd_out);
	if (params->fd[0] > 2)
		close(params->fd[0]);
	if (params->fd[1] > 2)
		close(params->fd[1]);
	if (state->prev_fd > 2)
		close(state->prev_fd);
}

void	setup_params_and_loop(t_command *cmd, t_data **data, char ***env,
		t_exec_params *params)
{
	t_pipeline_state	state;
	t_parent_params		parent_params;
	t_command			*curr;
	t_child_params		child_params;

	initialize_pipeline_state(params, &state);
	params->fd = (int [2]){0, 0};
	parent_params.prev_fd = &state.prev_fd;
	parent_params.fd = params->fd;
	parent_params.pids = (pid_t[1024]){0};
	parent_params.pid_count = &state.pid_count;
	child_params.cmd = cmd;
	child_params.data = data;
	child_params.env = env;
	curr = cmd;
	if (!setup_params_helper(&child_params, params, state, parent_params))
		return ;
	cleanup_pipeline_fds(params, &state);
	handle_core_dumped(parent_params.pids, state.pid_count, data);
	signal(SIGINT, my_handler);
}
