/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   meta_char.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 20:10:38 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 10:44:58 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	handle_pipe(t_token **current, t_command **current_cmd,
		t_command *first_cmd, t_data **data)
{
	t_command	*new_cmd;

	(void)first_cmd;
	if ((*current)->prev == NULL || (*current)->prev->type != TOKEN_WORD)
	{
		write(2, "minishell: syntax error\n", 25);
		set_status(2);
		return (0);
	}
	if ((*current)->next == NULL || (*current)->next->type == TOKEN_PIPE)
	{
		write(2, "minishell: syntax error\n", 25);
		set_status(2);
		return (0);
	}
	new_cmd = create_command();
	if (!new_cmd)
		return (0);
	(*current_cmd)->next = new_cmd;
	*current_cmd = new_cmd;
	*current = (*current)->next;
	(*data)->exit = 0;
	reset_redir_error(0);
	return (1);
}

int	handle_redir_in(t_token **current, t_command *cmd, t_data **data)
{
	(void)data;
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
	cmd_back(&(cmd->redir), cmd_new((*current)->next->av, (*current)->type, 0));
	(*current) = (*current)->next->next;
	return (1);
}

int	handle_redir_out(t_token **current, t_command *cmd, t_data **data)
{
	(void)data;
	if (!check_redir_syntax(current))
		return (0);
	if (get_redir_error())
	{
		(*current) = (*current)->next->next;
		return (1);
	}
	cmd_back(&(cmd->redir), cmd_new((*current)->next->av, (*current)->type,
			O_TRUNC));
	cmd->append = 0;
	(*current) = (*current)->next->next;
	return (1);
}

int	handle_redir_append(t_token **current, t_command *cmd, t_data **data)
{
	(void)data;
	if (!check_redir_syntax(current))
		return (0);
	if (get_redir_error())
	{
		(*current) = (*current)->next->next;
		return (1);
	}
	cmd_back(&(cmd->redir), cmd_new((*current)->next->av, (*current)->type,
			O_APPEND));
	cmd->append = 1;
	(*current) = (*current)->next->next;
	return (1);
}

int	handle_heredoc(t_token **current, t_command *cmd, int *i)
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
	if (heredoc_realloc(i, cmd, current) == 0)
		return (0);
	(*current) = (*current)->next->next;
	return (1);
}
