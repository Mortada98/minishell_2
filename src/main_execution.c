/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_execution.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:48:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/08 09:59:52 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	execute_command(t_command *cmd, char ***env, t_data **data)
{
	int		saved_stdin;
	bool	has_command;

	if (!cmd)
		return ;
	saved_stdin = dup(0);
	if (cmd && cmd->next == NULL && cmd->args && cmd->args[0]
		&& built_in(cmd->args[0]))
	{
		handle_single_builtin(cmd, env, data, saved_stdin);
		return ;
	}
	has_command = empty_command(cmd);
	if (!has_command)
	{
		handle_empty_command(cmd, data, saved_stdin, env);
		return ;
	}
	execute_pipeline_loop(cmd, data, env, saved_stdin);
	dup2(saved_stdin, 0);
	close(saved_stdin);
}
