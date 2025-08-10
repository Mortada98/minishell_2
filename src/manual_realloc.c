/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manual_realloc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:22:08 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/10 15:57:55 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

t_redir	*cmd_new(char *av, int type, int append)
{
	t_redir	*redir;

	redir = gc_malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->data = gc_strdup(av);
	redir->type = type;
	redir->append = append;
	redir->next = NULL;
	return (redir);
}

void	cmd_back(t_redir **redir, t_redir *new_redir)
{
	t_redir	*temp;

	if (!redir || !new_redir)
		return ;
	temp = *redir;
	if (!*redir)
	{
		*redir = new_redir;
		return ;
	}
	temp = *redir;
	while (temp && temp->next)
		temp = temp->next;
	temp->next = new_redir;
}

int	red_in_realloc(t_command *cmd, t_data **data, t_token **current)
{
	t_redir	*new_redir;

	new_redir = cmd_new((*current)->next->av, TOKEN_REDIR_IN, 0);
	if (!new_redir)
		return (0);
	cmd_back(&(cmd->redir), new_redir);
	(*data)->count_red_in++;
	return (1);
}

int	heredoc_realloc(int *i, t_command *cmd, t_token **current)
{
	char	**new_herdoc;
	int		j;

	new_herdoc = gc_malloc(sizeof(char *) * (*i + 2));
	if (!new_herdoc)
		return (0);
	if (cmd->herdoc)
	{
		j = -1;
		while (++j < *i)
		{
			new_herdoc[j] = cmd->herdoc[j];
		}
	}
	cmd->herdoc = new_herdoc;
	cmd->herdoc[(*i)] = gc_strdup((*current)->next->av);
	if (!cmd->herdoc[*i])
		return (0);
	if ((*current)->next->quoted)
		cmd->cmd_quoted = true;
	else
		cmd->cmd_quoted = false;
	cmd->herdoc[*i + 1] = NULL;
	(*i)++;
	return (1);
}

int	red_out_realloc(t_command *cmd, t_data **data, t_token **current)
{
	t_redir	*new_redir;
	int		append;

	append = 0;
	if ((*current)->type == TOKEN_REDIR_APPEND)
		append = 1;
	new_redir = cmd_new((*current)->next->av, (*current)->type, append);
	if (!new_redir)
		return (0);
	cmd_back(&(cmd->redir), new_redir);
	(*data)->count_red_out++;
	return (1);
}
