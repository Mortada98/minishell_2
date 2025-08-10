/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils_clean.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 22:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/10 18:00:22 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	initialize_pipeline_state(t_exec_params *params,
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
	// if (pid == 0)
	// 	handle_child_fork(child_params, params);
	// else if (pid > 0)
	// 	handle_parent_process(pid, child_params->curr, parent_params);
	// else
	// {
	// 	handle_fork_error(params->save, params->saved_stdin);
	// 	return (0);
	// }
	// if (params->save >= 0)
	// {
	// 	if (!child_params->curr->next)
	// 	{
	// 		close(parent_params->fd[0]);
	// 		close(parent_params->fd[1]);
	// 	}
	// 	dup2(params->save, 0);
	// 	close(params->save);
	// }
	return (1);
}

static void	cleanup_pipeline_fds(t_exec_params *params, t_pipeline_state *state)
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

int setup_params_helper(t_child_params *child_params, t_exec_params *params, t_pipeline_state state, t_parent_params parent_params)
{
	t_command *curr;

	curr = child_params->cmd;
	while (curr && !(child_params->cmd)->file)
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

// void init_fds(t_exec_params *params, t_parent_params *parent_params, t_pipeline_state *state)
// {
// 	params->fd[0] = 0;
// 	params->fd[1] = 0;
// 	parent_params->prev_fd = &state->prev_fd;
// 	parent_params->fd = params->fd;
// 	parent_params->pids = (pid_t[1024]){0};
// 	parent_params->pid_count = &state->pid_count;
// 	//child_params->cmd = cmd;
// 	//child_params->data = data;
// }

static void	setup_params_and_loop(t_command *cmd, t_data **data, char ***env,
		t_exec_params *params)
{
	t_parent_params		parent_params;
	t_child_params		child_params;
	t_pipeline_state	state;
	t_command			*curr;

	initialize_pipeline_state(params, &state);
	//init_fds(params, &parent_params, &state);
	params->fd = (int[2]){0, 0};
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
	// while (curr && !(cmd)->file)
	// {
	// 	child_params.curr = curr;
	// 	if (!process_pipeline_iteration(&child_params, params, &parent_params))
	// 	{
	// 		cleanup_pipeline_fds(params, &state);
	// 		dup2(params->saved_stdin, 0);
	// 		close(params->saved_stdin);
	// 		return ;
	// 	}
	// 	curr = curr->next;
	// }
	cleanup_pipeline_fds(params, &state);
	handle_core_dumped(parent_params.pids, state.pid_count, data);
	signal(SIGINT, my_handler);
}

void	execute_pipeline_loop(t_command *cmd, t_data **data, char ***env,
		int saved_stdin)
{
	t_exec_params	params;

	params.saved_stdin = saved_stdin;
	setup_params_and_loop(cmd, data, env, &params);
}
