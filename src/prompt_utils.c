/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 10:34:49 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 10:41:30 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

char	*prompt_helper(char **env)
{
	char	*result;
	char	*saved_cwd;

	saved_cwd = get_saved_cwd();
	if (saved_cwd)
	{
		result = gc_strdup(saved_cwd);
		free(saved_cwd);
		return (apply_tilde_replacement(result, env));
	}
	return (gc_strdup("(unknown)"));
}
