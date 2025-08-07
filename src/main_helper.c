/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 14:56:44 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/07 17:03:05 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	her_cmd_by_cmd(t_command *cmd, t_data *data, char **env,
		bool *heredoc_interrupted)
{
	t_command	*temp_cmd;
	int			herdoc_count;

	temp_cmd = cmd;
	while (temp_cmd && !*heredoc_interrupted)
	{
		if (temp_cmd->herdoc && temp_cmd->herdoc[0])
		{
			herdoc_count = 0;
			while (temp_cmd->herdoc[herdoc_count])
			{
				herdoc_count++;
			}
			data->count_herdoc = herdoc_count;
			excute_herdoc_for_child(&temp_cmd, &data, env);
			if (g_value == SIGINT)
			{
				*heredoc_interrupted = true;
				break ;
			}
		}
		temp_cmd = temp_cmd->next;
	}
}

int	interupte_herdoc(t_command *cmd, t_data *data, char **env)
{
	bool	heredoc_interrupted;

	heredoc_interrupted = false;
	her_cmd_by_cmd(cmd, data, env, &heredoc_interrupted);
	if (heredoc_interrupted)
	{
		g_value = 0;
		return (gc_cleanup_partial(), 1);
	}
	return (0);
}

int	process_line(t_token **token, char **line, char **env, t_data **data)
{
	*line = readline(prompt(env));
	if (!*line)
	{
		printf("exit\n");
		gc_cleanup();
		return (2);
	}
	gc_register_external(*line);
	if ((*line)[0] == '\0')
		return (0);
	add_history(*line);
	*token = tokenize(*line, data, env);
	if (!*token)
		return (0);
	return (1);
}
