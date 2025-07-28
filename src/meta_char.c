/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   meta_char.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:21:41 by helfatih          #+#    #+#             */
/*   Updated: 2025/07/28 16:42:25 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	handle_pipe(t_token **current, t_command **current_cmd,
		t_command *first_cmd, t_data **data)
{
	t_command	*new_cmd;

	if ((*current)->type != TOKEN_PIPE)
		return (1);
	if ((*current)->next == NULL)
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		(*data)->exit = 2;
		return (0);
	}
	new_cmd = create_command();
	if (!new_cmd)
	{
		free_cmd(first_cmd);
		return (0);
	}
	(*current_cmd)->next = new_cmd;
	*current_cmd = new_cmd;
	*current = (*current)->next;
	(*data)->exit = 0;
	return (1);
}

int	handle_redir_in(t_token **current, t_command *cmd, t_data **data)
{
	if (!(*current)->next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		set_status(2);
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token `%s'\n",
			(*current)->next->av);
		set_status(2);
		return (0);
	}
	if (red_in_realloc(cmd, data, current) == 0)
		return (0);
	(*current) = (*current)->next->next;
	return (1);
}

int	handle_redir_out(t_token **current, t_command *cmd)
{
	int	fd;

	if (!(*current)->next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		set_status(2);
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token `%s'\n",
			(*current)->next->av);
		set_status(2);
		return (0);
	}
	cmd->file_output = gc_strdup((*current)->next->av);
	if (!cmd->file_output)
		return (0);
	fd = open(cmd->file_output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (0);
	close(fd);
	cmd->append = 0;
	(*current) = (*current)->next->next;
	return (1);
}

int	handle_redir_append(t_token **current, t_command *cmd)
{
	int	fd;

	if (!(*current)->next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		set_status(2);
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token `%s'\n",
			(*current)->next->av);
		set_status(2);
		return (0);
	}
	cmd->file_output = gc_strdup((*current)->next->av);
	if (!cmd->file_output)
		return (0);
	fd = open(cmd->file_output, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		return (0);
	close(fd);
	cmd->append = 1;
	(*current) = (*current)->next->next;
	return (1);
}

int	handle_heredoc(t_token **current, t_command *cmd, int *i)
{
	if (!(*current)->next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		set_status(2);
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token `%s'\n",
			(*current)->next->av);
		set_status(2);
		return (0);
	}
	if (heredoc_realloc(i, cmd, current) == 0)
		return (0);
	(*current) = (*current)->next->next;
	return (1);
}
