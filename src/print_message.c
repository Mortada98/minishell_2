/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_message.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 17:12:58 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/08 21:12:02 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	unique_error(char *cmd, char *error)
{
	write(2, "minishell: ", 11);
	write(2, cmd, ft_strlen(cmd));
	write(2, ": ", 2);
	write(2, error, ft_strlen(error));
	write(2, "\n", 1);
	set_status(1);
}

char	*check_file(char *cmd)
{
	struct stat	st;
	DIR			*folder;

	if (stat(cmd, &st) == -1)
	{
		perror("minishell: ");
		set_status(127);
		return (NULL);
	}
	folder = opendir(cmd);
	if (folder != NULL)
	{
		print_message(cmd, 126, "minishell: ", ": Is a directory\n");
		closedir(folder);
		return (NULL);
	}
	closedir(folder);
	if (access(cmd, X_OK) == 0)
		return (gc_strdup(cmd));
	if (access(cmd, F_OK) == 0)
	{
		print_message(cmd, 126, "minishell: ", ": Permission denied\n");
		return (NULL);
	}
	return (NULL);
}

void	print_args(char **args, int idx)
{
	while (args[idx])
	{
		ft_putstr_fd(args[idx], 1);
		if (args[idx + 1])
			ft_putstr_fd(" ", 1);
		idx++;
	}
}

void	print_message(char *cmd, int status, char *format1, char *format2)
{
	write(2, format1, ft_strlen(format1));
	write(2, cmd, ft_strlen(cmd));
	write(2, format2, ft_strlen(format2));
	set_status(status);
}
