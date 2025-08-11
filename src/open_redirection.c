/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_redirection.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:44:30 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 11:59:47 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	open_red_in(int *fd_in, char *cmd)
{
	if (!fd_in || !cmd || !*cmd)
		return (0);
	if (*fd_in > 2)
		close(*fd_in);
	*fd_in = open(cmd, O_RDONLY);
	if (*fd_in < 0)
	{
		perror("minishell");
		set_status(1);
		return (0);
	}
	if (*fd_in > 2)
	{
		dup2(*fd_in, STDIN_FILENO);
		close(*fd_in);
	}
	return (1);
}

int	open_red_out(char *cmd, int *fd_out, char **env, int append)
{
	int	flags;

	(void)env;
	if (!fd_out || !cmd || !*cmd)
		return (0);
	if (is_directory_str(cmd))
		return (set_status(1), 0);
	flags = O_WRONLY | O_CREAT | append;
	if (*fd_out > 2)
		close(*fd_out);
	*fd_out = open(cmd, flags, 0644);
	if (*fd_out < 0)
	{
		perror("minishell");
		set_status(1);
		return (0);
	}
	if (*fd_out > 2)
	{
		dup2(*fd_out, STDOUT_FILENO);
		close(*fd_out);
	}
	return (1);
}

int	is_directory_str(char *cmd)
{
	DIR	*folder;

	folder = opendir(cmd);
	if (folder != NULL)
	{
		write(2, "minishell:  Is a directory\n", 28);
		set_status(1);
		closedir(folder);
		return (1);
	}
	return (0);
}

int	execute_red_of_child_check(t_command **cmd, int *fd_out, int *fd_in,
		char **env)
{
	t_redir	*temp;

	if ((*cmd)->redir)
	{
		temp = (*cmd)->redir;
		while (temp)
		{
			if (temp->type == TOKEN_REDIR_IN)
			{
				if (!open_red_in(fd_in, temp->data))
					return (0);
			}
			else if (temp->type == TOKEN_REDIR_OUT
				|| temp->type == TOKEN_REDIR_APPEND)
			{
				if (!open_red_out(temp->data, fd_out, env, temp->append))
					return (0);
			}
			temp = temp->next;
		}
	}
	return (1);
}

int	excute_redirection_of_child(t_command **cmd, t_data **data,
		t_exec_params *params, char **env)
{
	int	hd_fd;

	(void)data;
	if ((*cmd)->redir)
	{
		if (!execute_red_of_child_check(cmd, params->fd_out, params->fd_in,
				env))
			return (0);
	}
	if ((*cmd)->herdoc_file)
	{
		hd_fd = open((*cmd)->herdoc_file, O_RDONLY);
		if (hd_fd < 0)
		{
			write(2, "minishell: No such file or directory\n", 38);
			gc_cleanup();
			close_fds_except_std();
			exit(1);
		}
		dup2(hd_fd, 0);
		close(hd_fd);
	}
	return (1);
}
