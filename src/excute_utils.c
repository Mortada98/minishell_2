/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   excute_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:56:53 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/08 21:12:18 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	process_options(char **args, bool *has_n, int *idx)
{
	int	i;
	int	k;

	i = 1;
	while (args[i] && args[i][0] == '-')
	{
		if (args[i][1] == '\0')
			break ;
		k = 1;
		while (args[i][k] == 'n')
			k++;
		if (args[i][k] != '\0')
			break ;
		*has_n = true;
		i++;
	}
	*idx = i;
}

void	check_n_option(char *str, bool *has_n)
{
	int	i;

	if (ft_strncmp(str, "-n", 2) != 0)
		return ;
	i = 1;
	while (str[i] == 'n')
		i++;
	if (str[i] == '\0')
		*has_n = true;
}

void	my_echo(t_command *cmd)
{
	int		i;
	bool	has_n;

	has_n = false;
	i = 1;
	if (!cmd->args[i])
	{
		ft_putstr_fd("\n", 1);
		return ;
	}
	process_options(cmd->args, &has_n, &i);
	print_args(cmd->args, i);
	if (!has_n)
		ft_putstr_fd("\n", 1);
	set_status(0);
}

bool	built_in(char *cmd)
{
	if (ft_strcmp(cmd, "exit") == 0)
		return (true);
	if (ft_strcmp(cmd, "echo") == 0)
		return (true);
	if (ft_strcmp(cmd, "cd") == 0)
		return (true);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (true);
	if (ft_strcmp(cmd, "export") == 0)
		return (true);
	if (ft_strcmp(cmd, "unset") == 0)
		return (true);
	if (ft_strcmp(cmd, "env") == 0)
		return (true);
	else
	{
		return (false);
	}
}

void	free_2d_array(char **str)
{
	int	i;

	if (!str)
		return ;
	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}
