/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_setup_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 22:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2024/12/19 22:00:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	setup_pipe(t_command *curr, int *fd)
{
	if (curr->next)
	{
		if (pipe(fd) == -1)
		{
			perror("pipe");
			return (0);
		}
	}
	return (1);
}

void	handle_parent_process(pid_t pid, t_command *curr,
		t_parent_params *parent_params)
{
	if (*(parent_params->prev_fd) != -1)
		close(*(parent_params->prev_fd));
	if (curr->next)
	{
		close(parent_params->fd[1]);
		*(parent_params->prev_fd) = parent_params->fd[0];
	}
	parent_params->pids[*(parent_params->pid_count)] = pid;
	(*(parent_params->pid_count))++;
}

void	handle_fork_error(int save, int saved_stdin)
{
	perror("fork");
	close(save);
	dup2(saved_stdin, 0);
	close(saved_stdin);
}
