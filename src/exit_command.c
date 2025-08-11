/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 17:01:25 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 14:11:41 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	validation(t_command *cmd)
{
	if (cmd->args[1] && !is_number(cmd->args[1]))
	{
		write(2, "minishell: exit: numeric argument required\n", 44);
		return (2);
	}
	return (0);
}

int	make_exit(t_command *cmd)
{
	int	status;
	int	i;

	status = 0;
	i = 0;
	if (cmd->args[1] != NULL)
	{
		i = validation(cmd);
		if (i != 0)
			return (i);
		status = ft_atoi(cmd->args[1]) % 256;
		if (status < 0)
			status += 256;
		return (status);
	}
	return (0);
}

void	fill_error(t_command **cmd, int *error)
{
	int	i;

	i = make_exit(*cmd);
	if (i == 2)
	{
		set_status(2);
		*error = 1;
		return ;
	}
	if (i == 0)
	{
		set_status(get_status());
		*error = 1;
		return ;
	}
	set_status(i);
	*error = 1;
}

void	my_exit(t_command **cmd, t_data *data, int *error)
{
	int	arg_count;

	(void)data;
	if (ft_strcmp((*cmd)->args[0], "exit") == 0)
	{
		arg_count = 0;
		while ((*cmd)->args[arg_count])
			arg_count++;
		write(1, "exit\n", 5);
		if (arg_count > 2 && is_number((*cmd)->args[1]))
		{
			write(2, "minishell: exit: too many arguments\n", 36);
			set_status(1);
			*error = 0;
			return ;
		}
		fill_error(cmd, error);
	}
}

void	my_exit_child(t_command **cmd, t_data *data, int *error)
{
	int	arg_count;

	(void)data;
	if (ft_strcmp((*cmd)->args[0], "exit") == 0)
	{
		arg_count = 0;
		while ((*cmd)->args[arg_count])
			arg_count++;
		if (arg_count > 2 && is_number((*cmd)->args[1]))
		{
			write(2, "minishell: exit: too many arguments\n", 36);
			set_status(1);
			*error = 0;
			return ;
		}
		fill_error(cmd, error);
	}
}
