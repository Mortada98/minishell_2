#include "../include/minishell.h"

void  suprimer_quote(t_token **token)
{
  t_token *current;

  current = *token;
  while (current)
  {
    if (ft_strchr(current->av, '"'))
    {
      current->quoted = true;
      current->av = remove_quotes(current->av);
    }
    else
    {
      current->quoted = false;
    }
    current = current->next;
  }
}
