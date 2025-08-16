/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_cleanup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 00:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/16 21:17:03 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	close_fd2(int *saved_stdin1, int *saved_stdout1, t_fd *fd, char ***env)
{
	if (*saved_stdin1 >= 0)
		close(*saved_stdin1);
	if (*saved_stdout1 >= 0)
		close(*saved_stdout1);
	if (fd->fd_out >= 0)
		close(fd->fd_out);
	if (fd->saved_stdin >= 0)
		close(fd->saved_stdin);
	gc_cleanup();
	free_2d_array(*env);
	cleanup_exported_vars();
	rl_clear_history();
	exit(get_status());
}

void	close_all_fds(void)
{
	int	fd;

	fd = 3;
	while (fd < 256)
	{
		close(fd);
		fd++;
	}
}

void	close_fds_except_std(void)
{
	int	fd;

	fd = 3;
	while (fd < 256)
	{
		if (close(fd) == -1)
		{
			return ;
		}
		fd++;
	}
}
