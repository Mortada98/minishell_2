/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   suprimer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@1337.ma>                    +#+  +:+      
	+#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 10:00:47 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/16 10:01:19 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	suprimer_quote(t_token **token)
{
	t_token	*current;

	current = *token;
	while (current)
	{
		if (ft_strchr(current->av, '"') || ft_strchr(current->av, '\''))
		{
			current->quoted = true;
			current->av = remove_quotes(current->av);
		}
		else
		{
			current->quoted = false;
		}
		current = current->next;
	}
}
