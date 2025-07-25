/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:26:54 by helfatih          #+#    #+#             */
/*   Updated: 2025/07/24 14:33:59 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	free_token(t_token **token)
{
	t_token	*current;
	t_token	*next;

	current = *token;
	while (current)
	{
		next = current->next;
		if (current->av)
			free(current->av);
		free(current);
		current = next;
	}
}

int	is_space(char c)
{
	if (c == ' ')
		return (0);
	return (1);
}

int count_args(char **av)
{
  int i = 0;
  while (av[i])
  {
    i++;
  }
  return(i);
}

void  free_array(char **arr)
{
  int i;

  i = 0;
  while (arr[i])
  {
    free(arr[i]);
    i++;
  }
  free(arr);
}

void	free_cmd(t_command *cmd)
{
	t_command	*current, *next;
	int i;

	current = cmd;
	while (current)
	{
		next = current->next;
		if (current->args)
      free_array(current->args);
		if (current->file_input)
		{
			i = 0;
			while (current->file_input[i])
			{
				free(current->file_input[i]);
				i++;
			}
			free(current->file_input);
		}
		if (current->file_output)
			free(current->file_output);
		if (current->herdoc)
		{
			i = 0;
			while (current->herdoc[i])
			{
				free(current->herdoc[i]);
				i++;
			}
			free(current->herdoc);
		}
		if (current->herdoc_file)
		  free(current->herdoc_file);
		free(current);
		current = next;
	}
}

void	append_arg(t_command *cmd, char *str, t_data **data)
{
	int i, j;
	char	**new_array;
  (*data)->exit = 0;

	i = 0;
	if (cmd->args)
    i = count_args(cmd->args);
	new_array = ft_calloc(i + 2, sizeof(char *));
	if (!new_array)
		return;
	j = -1;
	while (++j < i)
		new_array[j] = cmd->args[j];
	new_array[i] = ft_strdup(str);
	if (!new_array[j])
		return(free(new_array));
	new_array[i + 1] = NULL;
	if (cmd->args)
		free(cmd->args);
	cmd->args = new_array;
}
