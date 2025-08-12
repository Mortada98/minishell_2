/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 10:34:49 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/12 12:52:01 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdio.h>

void	execve_error(void)
{
	if (errno == EISDIR)
	{
		set_status(127);
		write(2, "minishell: : Is a directory\n", 29);
	}
	else if (errno == ENOTDIR)
	{
		write(2, "minishell: : Not a directory\n", 30);
		set_status(126);
	}
	else if (errno == ENOENT)
	{
		set_status(127);
		write(2, "minishell: : No such file or directory\n", 40);
	}
	else if (errno == EACCES)
	{
		set_status(126);
		write(2, "minishell: : Permission denied\n", 32);
	}
  else
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
