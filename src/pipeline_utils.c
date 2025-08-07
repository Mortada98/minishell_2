/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 22:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/07 18:20:37 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

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

int	process_pipeline_iteration(t_child_params *child_params,
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
