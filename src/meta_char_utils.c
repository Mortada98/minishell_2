/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   meta_char_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 20:10:43 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 11:05:33 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	check_redir_syntax(t_token **current)
{
	if (!(*current)->next)
	{
		write(2, "minishell: syntax error\n", 25);
		set_status(2);
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		write(2, "minishell: syntax error\n", 25);
		set_status(2);
		return (0);
	}
	return (1);
}

void	print_open_error(char *filename)
{
	(void)filename;
	if (errno == EISDIR)
	{
		set_status(127);
		write(2, "minishell: Is a directory\n", 27);
	}
	else if (errno == ENOTDIR)
	{
		set_status(126);
		write(2, "minishell: Not a directory\n", 28);
	}
	else if (errno == ENOENT)
	{
		set_status(127);
		write(2, "minishell: No such file or directory\n", 38);
	}
	else if (errno == EACCES)
	{
		set_status(126);
		write(2, "minishell: Permission denied\n", 30);
	}
}
