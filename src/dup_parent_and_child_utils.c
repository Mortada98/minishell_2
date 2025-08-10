/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dup_parent_and_child_utils.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 18:24:07 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/10 18:24:12 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	print_errno(t_redir *temp)
{
	if (errno == EISDIR)
		print_message(temp->data, 1, "minishell: ",
			": Is a directory\n");
	else if (errno == ENOTDIR)
		print_message(temp->data, 1, "minishell: ",
			": Not a directory\n");
	else if (errno == ENOENT)
		print_message(temp->data, 1, "minishell: ",
			": No such file or directory\n");
	else if (errno == EACCES)
		print_message(temp->data, 1, "minishell: ",
			": Permission denied\n");
	else
		unique_error(temp->data, strerror(errno));
}

int	execute_red_child_check(t_builtin_params *param, int *fd_in)
{
	t_redir	*temp;

	if ((*param->curr)->redir)
	{
		temp = (*param->curr)->redir;
		while (temp)
		{
			if (temp->type == TOKEN_REDIR_IN)
			{
				if (!open_red_in(fd_in, temp->data))
					return (1);
			}
			temp = temp->next;
		}
	}
	return (0);
}
