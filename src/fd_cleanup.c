/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_cleanup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 00:00:00 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/07 19:02:34 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	close_all_fds(void)
{
	int	fd;
	int	max_fd;

	max_fd = sysconf(_SC_OPEN_MAX);
	if (max_fd == -1)
		max_fd = 1024;
	fd = 3;
	while (fd < max_fd)
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
			/* Ignore error if fd was already closed or doesn't exist */
		}
		fd++;
	}
}
