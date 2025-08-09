/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_execution.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:48:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/09 20:34:57 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	execute_command(t_command *cmd, char ***env, t_data **data)
{
	t_fd	fd;
	bool	has_command;

	if (!cmd)
		return ;
	fd.saved_stdin = dup(0);
	if (cmd && cmd->next == NULL && cmd->args && cmd->args[0]
		&& built_in(cmd->args[0]))
	{
		handle_single_builtin(cmd, env, data, &fd);
		return ;
	}
	has_command = empty_command(cmd);
	(void)has_command;
	execute_pipeline_loop(cmd, data, env, fd.saved_stdin);
	dup2(fd.saved_stdin, 0);
	close(fd.saved_stdin);
}
