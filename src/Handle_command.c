/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:26:30 by helfatih          #+#    #+#             */
/*   Updated: 2025/07/24 15:26:40 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <uchar.h>

t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = (t_command *)ft_calloc(1, sizeof(t_command));
	if(cmd == NULL)
	{
		return (NULL);
	}
	cmd->args = NULL;
	
	// Don't pre-allocate arrays - we'll allocate them as needed
	cmd->file_input = NULL;
	cmd->file_output = NULL;
	cmd->herdoc = NULL;
	cmd->herdoc_file = NULL;
	cmd->file = false;
	cmd->next = NULL;
	return (cmd);
}

void  make_the_envirement(char **result, char *valeur, size_t *old_size, size_t *new_size, bool flag, size_t *j)
{
  size_t len; 

  len = ft_strlen(valeur);
  if (valeur)
  {
    if (*j + len >= *old_size)
    {
      *new_size = len + (*j);
      *result = manual_realloc(*result, *new_size);
      *old_size = *new_size;
    }
    if (flag)
    {
      make_like_bash(*result, valeur, j);
    }
    else
    {
      ft_strlcpy(&(*result)[*j], valeur, len + 1);
      *j += len;
    }
  }
}

char *split_var(size_t *i, char *str, size_t *start)
{
  char *string;

  (*i)++;
  *start = *i;
  while((ft_isalnum(str[*i]) || str[*i] == '_') && str[*i])
    (*i)++;
  string = ft_substr(str, *start, (*i) - (*start));	
  if (!string)
    return (NULL);
  return(string);
}

char	*expand_env(char *str)
{
	char	*result, *string, *valeur;
  bool condition, flag;
	size_t	(i), j, old_size, new_size, start;

  init_var(str, &i, &j, &old_size, &condition, &flag);
	if(!(result = (char *)ft_calloc((old_size), 1)))
		return (NULL);
	while (str[i])
	{
		if (condition && str[i] == '$' && (ft_isalnum(str[i + 1]) || str[i + 1] == '_') && str[i + 1])
		{
      string = split_var(&i, str, &start);
      if (!string)
        return(NULL);
			valeur = getenv(string);
			free(string);
			if(valeur)
			{
        make_the_envirement(&result, valeur, &old_size, &new_size, flag, &j);
			  continue;
			}
		}	
		result[j++] = str[i++];
	}
  return (result);
}


void  how_many(t_token *token, t_data **data)
{
  t_token *cur;

  // Reset counts
  (*data)->count_herdoc = 0;
  (*data)->count_red_in = 0;
  
  cur = token;
  while (cur)
  {
    if (cur->type == TOKEN_HERDOC)
      (*data)->count_herdoc++;
    if (cur->type == TOKEN_REDIR_IN)
      (*data)->count_red_in++;
    cur = cur->next;
  }
}

t_command *parsing_command(t_token *token, t_data **data)
{
  t_token *current = token;
  t_command *first_cmd;
  t_command *current_cmd ;

  // Don't pre-count, we'll allocate as needed
  current_cmd = create_command();
  if (!current_cmd)
    return (NULL);
  first_cmd = current_cmd;
  (*data)->count_red_in = 0;  // Reset for actual parsing
  int i = 0;  // heredoc index for current command
  
  while (current)
  {
    if (current->type == TOKEN_PIPE)
    {
      // Reset indices for new command
      i = 0;
      (*data)->count_red_in = 0;
      if (!handle_pipe(&current, &current_cmd, first_cmd, data))
        return (NULL);
    }
    else if (current->type == TOKEN_REDIR_IN)
    {
      if (!handle_redir_in(&current, current_cmd, first_cmd, data))
        return (NULL);
    }
    else if (current->type == TOKEN_REDIR_OUT)
    {
      if (!handle_redir_out(&current, current_cmd, first_cmd, data))
        return (NULL);
    }
    else if (current->type == TOKEN_REDIR_APPEND)
    {
      if (!handle_redir_append(&current, current_cmd, first_cmd, data))
        return (NULL);
    }
    else if (current->type == TOKEN_HERDOC)
    {
      if (!handle_heredoc(&current, current_cmd, first_cmd, data, &i))
        return(NULL);
    }
    else if (current->type == TOKEN_WORD)
    {
      append_arg(current_cmd, current->av, data);
      current = current->next;
    }
  }
  
  return (first_cmd);
}

