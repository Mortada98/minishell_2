/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 15:10:54 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/12 20:54:52 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	lexe_with_space(t_token **token, int *start, int *i, char *word)
{
	char	*str;
	bool	join;

	if (!word)
		return ;
	str = NULL;
	join = false;
	if (word[*i] == 0)
		join = true;
	if (*i > *start)
	{
		str = gc_substr(word, *start, *i - *start);
		if (str)
			add_token(token, creat_token(str, TOKEN_WORD, join, false));
	}
	while ((word[*i] == ' ' || word[*i] == '\t'))
		(*i)++;
	*start = *i;
}

void	join_expansion(char *str, t_token **token)
{
	t_token	*cur;

	cur = *token;
	if (!*token)
		return ;
	if (str[0] == ' ')
	{
		while ((*token)->next)
		{
			(*token) = (*token)->next;
		}
		(*token)->info = false;
		*token = cur;
	}
}

char	*make_content(char *line, t_data **data)
{
	char	*word;

	if (!line || (*data)->start < 0 || (*data)->end < (*data)->start)
		return (NULL);
	word = gc_substr(line, (*data)->start, (*data)->end - (*data)->start);
	if (!word)
		return (NULL);
	return (word);
}
