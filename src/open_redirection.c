/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_redirection.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:44:30 by helfatih          #+#    #+#             */
/*   Updated: 2025/07/28 17:17:52 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	open_red_in(t_data **data, int *fd_in, t_command **cmd)
{
	int	i;

	i = 0;
	while (i < (*data)->count_red_in)
	{
		*fd_in = open((*cmd)->file_input[i], O_RDONLY);
		if (*fd_in < 0)
		{
			printf("minishell: %s: No such file or directory\n",
				(*cmd)->file_input[i]);
			set_status(1);
			exit(1);
		}
		dup2(*fd_in, STDIN_FILENO);
		close(*fd_in);
		i++;
	}
}

void	open_red_out(t_command **cmd, int *fd_out)
{
    int flags;
    
	if (is_directory(cmd))
		exit(1);
	flags = O_WRONLY | O_CREAT | append_or_trunc(cmd);
	*fd_out = open((*cmd)->file_output, flags, 0644);
	if (*fd_out < 0)
	{
		perror("minishell");
		set_status(1);
		exit(1);
	}
	dup2(*fd_out, STDOUT_FILENO);
	close(*fd_out);
}

int	is_directory(t_command **cmd)
{
	DIR	*folder;

	folder = opendir((*cmd)->file_output);
	if (folder != NULL)
	{
		printf("minishell : %s:  Is a directory\n", (*cmd)->file_output);
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
		int *fd_in)
{
	if ((*cmd)->file_input)
	{
		open_red_in(data, fd_in, cmd);
	}
	if ((*cmd)->file_output)
	{
		open_red_out(cmd, fd_out);
	}
}
