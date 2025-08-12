/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_files.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 10:02:14 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 10:27:31 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	input_file(int *fd_in, char *file)
{
	if (*fd_in > 2)
		close(*fd_in);
	*fd_in = open(file, O_RDONLY);
	if (*fd_in < 0)
	{
		print_open_error(file);
		return (0);
	}
	if (dup2(*fd_in, STDIN_FILENO) == -1)
	{
		perror("minishell");
		close(*fd_in);
		return (0);
	}
	close(*fd_in);
	return (1);
}

int	output_file(int *flags, char *file, int *fd_out)
{
	*flags = O_WRONLY | O_CREAT | O_TRUNC;
	*fd_out = open(file, *flags, 0644);
	if (*fd_out < 0)
	{
		print_open_error(file);
		return (0);
	}
	if (dup2(*fd_out, STDOUT_FILENO) == -1)
	{
		perror("minishell");
		close(*fd_out);
		return (0);
	}
	close(*fd_out);
	return (1);
}

int	append_file(int *flags, char *file, int *fd_out)
{
	*flags = O_WRONLY | O_CREAT | O_APPEND;
	*fd_out = open(file, *flags, 0644);
	if (*fd_out < 0)
	{
		print_open_error(file);
		return (0);
	}
	if (dup2(*fd_out, STDOUT_FILENO) == -1)
	{
		perror("minishell");
		close(*fd_out);
		return (0);
	}
	close(*fd_out);
	return (1);
}
