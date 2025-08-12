/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 18:13:22 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 20:54:52 by mbouizak         ###   ########.fr       */
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

int	count_active_pids(pid_t *pids, int command_count)
{
	int	count;
	int	i;

	if (!pids || command_count <= 0)
		return (0);
	count = 0;
	i = 0;
	while (i < command_count)
	{
		if (pids[i] != 0)
			count++;
		i++;
	}
	return (count);
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

int	count_pipeline_commands(t_command *cmd)
{
	int			count;
	t_command	*current;

	if (!cmd)
		return (0);
	count = 0;
	current = cmd;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

void	setup_params_and_loop(t_command *cmd, t_data **data, char ***env,
		t_exec_params *params)
{
	t_pipeline_state	state;
	t_parent_params		parent_params;
	t_child_params		child_params;
	int					*pipe_fds;
	pid_t				*pids;
	int					command_count;

	initialize_pipeline_state(params, &state);
	command_count = count_pipeline_commands(cmd);
	if (command_count == 0)
		return ;
	pipe_fds = gc_malloc(2 * sizeof(int));
	if (!pipe_fds)
		return ;
	pipe_fds[0] = 0;
	pipe_fds[1] = 0;
	params->fd = pipe_fds;
	parent_params.prev_fd = &state.prev_fd;
	parent_params.fd = params->fd;
	pids = gc_calloc(command_count, sizeof(pid_t));
	if (!pids)
		return ;
	parent_params.pids = pids;
	parent_params.pid_count = &state.pid_count;
	child_params.cmd = cmd;
	child_params.data = data;
	child_params.env = env;
	if (!setup_params_helper(&child_params, params, state, parent_params))
		return ;
	cleanup_pipeline_fds(params, &state);
	handle_core_dumped(parent_params.pids, state.pid_count, data);
	signal(SIGINT, my_handler);
}
