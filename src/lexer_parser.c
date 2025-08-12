/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 20:41:03 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 20:54:52 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	check_next_element(t_token *cur, t_data **data)
{
	while (cur->next)
	{
		cur = cur->next;
	}
	if (cur->type == TOKEN_HERDOC)
		(*data)->should_expand_outside = true;
}

void	make_list(char *word, t_token **token)
{
	int		start;
	int		i;

	if (!word)
		return ;
	i = 0;
	start = 0;
	while (word[i])
	{
		if (word[i] == ' ' || word[i] == '\t')
		{
			lexe_with_space(token, &start, &i, word);
			continue ;
		}
		i++;
	}
	lexe_with_space(token, &start, &i, word);
}

void	check_the_last_element(t_token **token, t_data **data)
{
	t_token	*cur;

	cur = *token;
	if (!*token)
		return ;
	if (cur->type == TOKEN_HERDOC)
	{
		(*data)->should_expand_outside = true;
		return ;
	}
	else if (cur->next)
	{
		check_next_element(cur, data);
	}
}
