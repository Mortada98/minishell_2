/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:54:31 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 13:52:20 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	is_number(char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	if (str[i] == '\0')
		return (0);
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

void    parent_status(int status, int flags, int sig)
{
  if (WIFSIGNALED(status))
  {
    sig = WTERMSIG(status);
    if (sig == SIGINT && !flags)
    {
      set_status(128 + sig);
      write(1, "\n", 1);
    }
    else if (sig == SIGQUIT)
      write(2, "Quit (core dumped)\n", 19);
    set_status(128 + sig);
  }
}

void	handle_core_dumped(int *pids, int pid_count, t_data **data)
{
	int (i), status, sig, flags;
	sig = 0;
  flags = 0;
	signal(SIGINT, SIG_IGN);
	i = -1;
  (void)data;
	while (++i < pid_count)
	{
		waitpid(pids[i], &status, 0);
    sig = WTERMSIG(status);
    if (sig == SIGINT && pid_count != 1 && !flags)
    {
      set_status(128 + sig);
      write(1, "\n", 1);
      flags = 1;
    }
		if (i == pid_count - 1)
			set_status(WEXITSTATUS(status));
	}
  parent_status(status, flags, sig);
}
void	empty_command(t_command *cmd)
{
	t_command	*temp_cmd;

	temp_cmd = cmd;
	if (!temp_cmd)
		return ;
	while (temp_cmd)
	{
		if (temp_cmd->args && temp_cmd->args[0][0] == '\0')
		{
			write(2, "minishell : '' : command not found\n", 36);
		}
		temp_cmd = temp_cmd->next;
	}
}

int	access_file(t_command *cmd)
{
	int		fd;
	t_redir	*redir;

	redir = cmd->redir;
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_IN)
		{
			fd = open(redir->data, O_RDONLY);
			if (fd < 0)
			{
				write(2, "minishell: No such file or directory\n", 38);
				set_status(1);
				return (0);
			}
			close(fd);
		}
		redir = redir->next;
	}
	return (1);
}

void	my_handler(int sig)
{
	if (sig == SIGINT)
	{
		write(1, "^C\n", 3);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		set_status(130);
	}
}
