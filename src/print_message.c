/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_message.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 17:12:58 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 12:10:25 by helfatih         ###   ########.fr       */
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

void	set_errno(int *fd_out, t_redir **cmd, char **env)
{
	(void)cmd;
	if (*fd_out < 0)
	{
		if (errno == ENOTDIR)
			write(2, "minishell: Not a directory\n", 28);
		else if (errno == ENOENT)
			write(2, "minishell: No such file or directory\n", 38);
		else if (errno == EACCES)
			write(2, "minishell: Permission denied\n", 30);
		else if (errno == EISDIR)
			write(2, ": Is a directory\n", 17);
		else
			write(2, "minishell: No such file or directory\n", 38);
		set_status(1);
		gc_cleanup();
		free_2d_array(env);
		close_fds_except_std();
		exit(1);
	}
}

void	print_message(char *cmd, int status, char *format1, char *format2)
{
	char	*first_join;
	char	*second_join;

	first_join = gc_strjoin(format1, cmd);
	if (!first_join)
		return ;
	second_join = gc_strjoin(first_join, format2);
	if (second_join == NULL)
		return ;
	write(2, second_join, ft_strlen(second_join));
	set_status(status);
}
