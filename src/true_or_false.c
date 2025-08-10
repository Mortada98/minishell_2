/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   true_or_false.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:13:25 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/10 18:34:33 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	is_directory(t_command **cmd)
{
	t_redir	*temp;

	if (!cmd || !*cmd)
		return (0);
	temp = (*cmd)->redir;
	while (temp)
	{
		if (temp->type == TOKEN_REDIR_OUT || temp->type == TOKEN_REDIR_APPEND)
		{
			if (is_directory_str(temp->data))
				return (1);
		}
		temp = temp->next;
	}
	return (0);
}

bool	*init_bool(void)
{
	static bool	var = false;

	return (&var);
}

void	set_bool(bool val)
{
	bool	*status;

	status = init_bool();
	*status = val;
}

bool	get_bool(void)
{
	return (*init_bool());
}
