/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_execution_helpers.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 22:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2024/12/19 22:00:00 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	cleanup_and_exit(int save, int saved_stdin, char ***env, int status)
{
	dup2(save, 0);
	close(save);
	close(saved_stdin);
	close_fds_except_std();
	gc_cleanup();
	if ((*env)[0])
		free_2d_array(*env);
	rl_clear_history();
	exit(status);
}

static void	execute_builtin_child(t_builtin_params *bp)
{
	excute_redirection_of_child_builtin(bp->curr, bp->fd_out, bp->data,
		bp->saved_stdin, bp->save, bp->env);
	cleanup_and_exit(*(bp->save), *(bp->saved_stdin), bp->env, get_status());
}

static void	execute_external_command(t_command *curr, char ***env,
	int save, int saved_stdin)
{
	char	*command;

	command = get_command(curr->args[0], *env);
	if (!command)
		cleanup_and_exit(save, saved_stdin, env, 127);
	if (execve(command, curr->args, *env) == -1)
		cleanup_and_exit(save, saved_stdin, env, 126);
}

void	execute_command_logic(t_command *curr, t_child_params *child_params,
	t_exec_params *params, t_builtin_params *bp)
{
	if (curr->args && curr->args[0] && curr->args[0][0] != '\0')
	{
		if (built_in(curr->args[0]))
		{
			bp->curr = &curr;
			bp->fd_out = params->fd_out;
			bp->data = *(child_params->data);
			bp->saved_stdin = &params->saved_stdin;
			bp->save = &params->save;
			bp->env = child_params->env;
			execute_builtin_child(bp);
		}
		execute_external_command(curr, child_params->env, params->save,
			params->saved_stdin);
	}
	else
	{
		if (!params->fd_in)
		{
			static int stdin_fd = STDIN_FILENO;
			params->fd_in = &stdin_fd;
		}
		excute_redirection_of_child(&curr, child_params->data, params->fd_out, params->fd_in);
		dup2(params->save, 0);
		cleanup_and_exit(params->save, params->saved_stdin, child_params->env,
			0);
	}
}
