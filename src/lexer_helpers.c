/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 21:34:54 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/16 10:23:36 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

char	*handle_expansion(t_data *data, char *word, char **env)
{
	char	*str;

	if (!data->should_expand_outside)
	{
		str = expand_env(word, env, false);
		return (str);
	}
	return (word);
}

void	create_add_token(t_token **token, char *word, t_token_type value,
		t_word_processing *wp)
{
	t_token	*new;

	new = creat_token(word, value, wp->should_join, wp->found);
	if (new)
		add_token(token, new);
}

void	process_word(t_token **token, t_word_processing *wp)
{
	join_expansion(wp->str, token);
	wp->str = wp->str;
	if (wp->str && wp->str != wp->word)
		wp->word = wp->str;
	if (!wp->flag)
		wp->value = get_token_type(wp->word);
	if (wp->word[0] != '\'' && wp->word[0] != '"' && (ft_strchr(wp->word, ' ')
			|| ft_strchr(wp->word, '\t')))
	{
		make_list(wp->word, token);
		return ;
	}
	create_add_token(token, wp->word, wp->value, wp);
}
