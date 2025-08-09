/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_execution.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 22:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2024/12/19 22:00:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	setup_child_io(int prev_fd, int *fd, t_command *curr)
{
	if (curr->next)
	{
		dup2(fd[1], 1);
		close(fd[1]);
		close(fd[0]);
	}
	if (prev_fd != -1)
	{
		dup2(prev_fd, 0);
		close(prev_fd);
	}
}

void	execute_child_process(t_command *curr, t_data **data,
		t_exec_params *params, char ***env)
{
	t_builtin_params	bp;
	t_child_params		child_params;

	child_params.data = data;
	child_params.env = env;
	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	setup_child_io(params->prev_fd, params->fd, curr);
	excute_redirection_of_child(&curr, data, params->fd_out, params->fd_in, *env);
	execute_command_logic(curr, &child_params, params, &bp);
}
