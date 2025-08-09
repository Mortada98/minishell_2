/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_redirection.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:44:30 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/09 20:41:18 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	open_red_in(int *fd_in, t_command **cmd)
{
	int	i;

	if (!fd_in || !cmd || !*cmd)
		return ;
	if (!(*cmd)->file_input)
		return ;
	i = 0;
	while ((*cmd)->file_input && (*cmd)->file_input[i])
	{
		if (*fd_in > 2)
			close(*fd_in);
		*fd_in = open((*cmd)->file_input[i], O_RDONLY);
		if (*fd_in < 0)
		{
			if (!(*cmd)->redir_error)
			{
				write(2, "minishell: ", 11);
				write(2, (*cmd)->file_input[i],
					ft_strlen((*cmd)->file_input[i]));
				if (errno == ENOTDIR)
					write(2, ": Not a directory\n", 18);
				else if (errno == ENOENT)
					write(2, ": No such file or directory\n", 28);
				else if (errno == EACCES)
					write(2, ": Permission denied\n", 20);
				else
					write(2, ": No such file or directory\n", 28);
				(*cmd)->redir_error = true;
				set_status(1);
			}
			i++;
			continue ;
		}
		dup2(*fd_in, STDIN_FILENO);
		close(*fd_in);
		i++;
	}
}

void	set_errno(int *fd_out, t_command **cmd, char **env)
{
	if (*fd_out < 0)
	{
		write(2, "minishell: ", 11);
		write(2, (*cmd)->file_output, ft_strlen((*cmd)->file_output));
		if (errno == ENOTDIR)
			write(2, ": Not a directory\n", 18);
		else if (errno == ENOENT)
			write(2, ": No such file or directory\n", 28);
		else if (errno == EACCES)
			write(2, ": Permission denied\n", 20);
		else if (errno == EISDIR)
			write(2, ": Is a directory\n", 17);
		else
			write(2, ": No such file or directory\n", 28);
		set_status(1);
		gc_cleanup();
		free_2d_array(env);
		close_fds_except_std();
		exit(1);
	}
}

void	open_red_out(t_command **cmd, int *fd_out, char **env)
{
	int	flags;

	if (!fd_out)
		return ;
	if (is_directory(cmd))
	{
		gc_cleanup();
		close_fds_except_std();
		exit(1);
	}
	flags = O_WRONLY | O_CREAT | append_or_trunc(cmd);
	*fd_out = open((*cmd)->file_output, flags, 0644);
	set_errno(fd_out, cmd, env);
	dup2(*fd_out, STDOUT_FILENO);
	close(*fd_out);
}

int	is_directory(t_command **cmd)
{
	DIR	*folder;

	folder = opendir((*cmd)->file_output);
	if (folder != NULL)
	{
		write(2, "minishell : ", 12);
		write(2, (*cmd)->file_output, ft_strlen((*cmd)->file_output));
		write(2, ":  Is a directory\n", 18);
		set_status(1);
		closedir(folder);
		return (1);
	}
	closedir(folder);
	return (0);
}

int	append_or_trunc(t_command **cmd)
{
	if ((*cmd)->append)
	{
		return (O_APPEND);
	}
	else
		return (O_TRUNC);
}

void	excute_redirection_of_child(t_command **cmd, t_data **data, int *fd_out,
		int *fd_in, char **env)
{
	int	hd_fd;

	(void)data;
	if ((*cmd)->file_output)
	{
		open_red_out(cmd, fd_out, env);
	}
	if ((*cmd)->file_input)
	{
		open_red_in(fd_in, cmd);
		if ((*cmd)->redir_error)
			return ;
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
}
