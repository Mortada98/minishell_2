/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_files.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 10:02:14 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/15 11:10:28 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

t_exported_var	**get_exported_vars_ptr(void)
{
	static t_exported_var	*exported_vars = NULL;

	return (&exported_vars);
}

void	print_open_error_parent(char *filename)
{
	(void)filename;
	if (errno == EISDIR)
	{
		set_status(1);
		write(2, "minishell: Is a directory\n", 27);
	}
	else if (errno == ENOTDIR)
	{
		set_status(1);
		write(2, "minishell: Not a directory\n", 28);
	}
	else if (errno == ENOENT)
	{
		set_status(1);
		write(2, "minishell: No such file or directory\n", 38);
	}
	else if (errno == EACCES)
	{
		set_status(1);
		write(2, "minishell: Permission denied\n", 30);
	}
}

int	input_file(int *fd_in, char *file, int status)
{
	if (*fd_in > 2)
		close(*fd_in);
	*fd_in = open(file, O_RDONLY);
	if (*fd_in < 0)
	{
		if (status == 1)
			print_open_error_parent(file);
		else
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

int	output_file(int *flags, char *file, int *fd_out, int status)
{
	*flags = O_WRONLY | O_CREAT | O_TRUNC;
	*fd_out = open(file, *flags, 0644);
	if (*fd_out < 0)
	{
		if (status == 1)
			print_open_error_parent(file);
		else
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

int	append_file(int *flags, char *file, int *fd_out, int status)
{
	*flags = O_WRONLY | O_CREAT | O_APPEND;
	*fd_out = open(file, *flags, 0644);
	if (*fd_out < 0)
	{
		if (status == 1)
			print_open_error_parent(file);
		else
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
