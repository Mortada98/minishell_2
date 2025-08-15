/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:26:13 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/11 18:19:09 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	parsing_process(t_token **token, char **env, t_data *data, t_command **cmd)
{
	int	check;

	join_nodes(token);
  suprimer_quote(token);
	*cmd = parsing_command(*token, &data);
	if (!*cmd)
		return (0);
	check = interupte_herdoc(*cmd, data, env);
	if (check == 1)
		return (0);
	return (1);
}

void	part_execution(t_command **cmd, char ***env, t_data **data)
{
	g_value = 0;
	if (*cmd)
	{
		execute_command(*cmd, env, data);
		if ((*cmd)->herdoc_file)
			unlink((*cmd)->herdoc_file);
	}
	close_fds_except_std();
	gc_cleanup_partial();
}

void	clean_up(char **env)
{
	gc_cleanup();
	if (env)
	{
		free_2d_array(env);
	}
	rl_clear_history();
}

void	make_prompt(char ***env)
{
	char		*line;
	t_token		*token;
	t_command	*cmd;
	t_data		*data;
	int			refer;

	while (1)
	{
		data = gc_malloc(sizeof(t_data));
		if (!data)
			return ;
		ft_memset(data, 0, sizeof(t_data));
		refer = process_line(&token, &line, *env, &data);
		if (refer == 0)
			continue ;
		if (refer == 2)
			break ;
		if (!parsing_process(&token, *env, data, &cmd))
			continue ;
		part_execution(&cmd, env, &data);
	}
	clean_up(*env);
}

int	main(int ac, char **av, char **env)
{
	char	**my_env;

	(void)ac;
	(void)av;
	rl_catch_signals = 0;
	signal(SIGINT, my_handler);
	signal(SIGQUIT, SIG_IGN);
	init_saved_cwd();
	my_env = copy_env(env);
	make_prompt(&my_env);
	cleanup_saved_cwd();
	cleanup_exported_vars();
	return (0);
}
