/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 10:34:49 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 22:24:36 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdio.h>

void	execve_error(void)
{
	if (errno == ENOTDIR)
	{
		write(2, "minishell: : Not a directory\n", 30);
		set_status(126);
	}
	else if (errno == EACCES)
	{
		set_status(126);
		write(2, "minishell: : Permission denied\n", 32);
	}
	else if (errno != ENOTDIR && errno != EACCES
		&& get_status() != 100 && errno != 0 && get_status() != 200)
	{
		perror("minishell");
		set_status(127);
	}
	else if (get_status() == 100)
		set_status(0);
}

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
