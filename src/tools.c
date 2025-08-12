/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 16:36:39 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 20:54:52 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

char	*manual_realloc(char *old, size_t old_len, size_t new_len)
{
	char	*new;

	new = gc_malloc(new_len + 1);
	if (new == NULL)
		return (NULL);
	if (old && old_len > 0)
	{
		ft_memcpy(new, old, old_len);
		new[old_len] = '\0';
	}
	else
		new[0] = '\0';
	return (new);
}

bool	con(char *str)
{
	if (str[0] == '\'')
		return (false);
	else
		return (true);
}

bool	flaging(char *str)
{
	if (str[0] != '\'' && str[0] != '\"')
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

void	make_like_bash(char *result, char *valeur, size_t *j)
{
	int	k;

	k = 0;
	while (valeur[k])
	{
		if (valeur[k] == ' ' || valeur[k] == '\t')
		{
			while (valeur[k] == ' ' || valeur[k] == '\t')
			{
				k++;
			}
			result[(*j)++] = ' ';
		}
		else
		{
			result[(*j)++] = valeur[k++];
		}
	}
}

int	init_var(char *str, t_var *var)
{
	if (!str)
		return (0);
	var->flag = false;
	var->old_size = (ft_strlen(str) + 2 + 100);
	var->i = 0;
	var->j = 0;
	var->condition = con(str);
	var->flag = flaging(str);
	var->result = (char *)gc_calloc((var->old_size), 1);
	if (var->result == NULL)
	{
		return (0);
	}
	return (1);
}
