/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_execution_old.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:48:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/09 10:35:39 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	setup_child_io(int prev_fd, int *fd, t_command *curr)
{
	int	hd_fd;

	if (prev_fd != -1)
	{
		dup2(prev_fd, 0);
		close(prev_fd);
	}
	if (!(curr->file) && curr->herdoc_file)
	{
		hd_fd = open(curr->herdoc_file, O_RDONLY);
		if (hd_fd < 0)
		{
			write(2, "minishell: No such file or directory\n", 36);
			exit(1);
		}
		dup2(hd_fd, 0);
		close(hd_fd);
	}
	if (curr->next)
	{
		close(fd[0]);
		dup2(fd[1], 1);
		close(fd[1]);
	}
}

static void	cleanup_and_exit(int save, int saved_stdin, char ***env, int status)
{
	close(save);
	close(saved_stdin);
	gc_cleanup();
	if ((*env)[0])
		free_2d_array(*env);
	rl_clear_history();
	close_fds_except_std();
	exit(status);
}

static void	execute_builtin_child(t_builtin_params *bp)
{
	excute_redirection_of_child_builtin(bp);
	cleanup_and_exit(*(bp->save), *(bp->saved_stdin), bp->env, get_status());
}

static void	execute_external_command(t_command *curr, char ***env,
	int save, int saved_stdin)
{
	char	*command;

	command = get_command(curr->args[0], *env);
	if (!command)
		cleanup_and_exit(save, saved_stdin, env, get_status());
	if (execve(command, curr->args, *env) != 0)
	{
		perror("execve");
		cleanup_and_exit(save, saved_stdin, env, 1);
	}
}

static void	execute_command_logic(t_command *curr, t_child_params *child_params,
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
		dup2(params->save, 0);
		cleanup_and_exit(params->save, params->saved_stdin, child_params->env,
			0);
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
	excute_redirection_of_child(&curr, data, params->fd_out, params->fd_in);
	if (curr->redir_error)
		cleanup_and_exit(params->save, params->saved_stdin, env, 1);
	execute_command_logic(curr, &child_params, params, &bp);
}
