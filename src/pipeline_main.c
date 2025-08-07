/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_main.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 22:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/07 18:48:36 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	execute_pipeline_loop(t_command *cmd, t_data **data, char ***env,
	int saved_stdin)
{
	t_command	*curr;
	int			vars[2];
	int			fd[2];
	pid_t		pids[1024];

	curr = cmd;
	vars[0] = -1;
	vars[1] = 0;
	while (curr && !(cmd)->file)
	{
		if (!process_pipeline_iteration(&(t_child_params){cmd, curr, data, env},
			&(t_exec_params){vars[0], fd, NULL, NULL, 0, saved_stdin},
			&(t_parent_params){&vars[0], fd, pids, &vars[1]}))
		{
			dup2(saved_stdin, 0);
			close(saved_stdin);
			return ;
		}
		curr = curr->next;
	}
	handle_core_dumped(pids, vars[1], data);
	signal(SIGINT, my_handler);
	dup2(saved_stdin, 0);
	close(saved_stdin);
}
