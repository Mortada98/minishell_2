/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manage_cwd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 18:55:27 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 10:26:44 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	parent_status(int status, int flags, int sig)
{
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT && !flags)
		{
			set_status(128 + sig);
			write(1, "\n", 1);
		}
		else if (sig == SIGQUIT)
			write(2, "Quit (core dumped)\n", 19);
		set_status(128 + sig);
	}
}

void	init_saved_cwd(void)
{
	manage_saved_cwd(2, NULL);
}

void	update_saved_cwd_public(void)
{
	manage_saved_cwd(2, NULL);
}

char	*get_saved_cwd(void)
{
	return (manage_saved_cwd(0, NULL));
}

void	cleanup_saved_cwd(void)
{
	manage_saved_cwd(3, NULL);
}
