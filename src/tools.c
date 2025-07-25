#include "../include/minishell.h"

char	*manual_realloc(char *old, size_t len)
{
  char *new = malloc(len + 1);
  if (new == NULL)
    return (NULL);
  if (old)
  {
    ft_memcpy(new, old, len);
    free(old);
  }
  new[len] = '\0';
  return (new);
}

bool  con(char *str)
{
  if (str[0] == '\'')
    return(false);
  else
    return(true);
}

bool  flaging(char *str)
{
  if (str[0] != '\'' && str[0] != '\"')
  {
    return (true);
  }
  else
  {
    return(false);
  }
}

void  make_like_bash(char *result, char *valeur, size_t *j)
{
  int k = 0;
  while (valeur[k])
  {
    if (valeur[k] == ' ')
    {
      while(valeur[k] == ' ')
      {
        k++;
      }
      result[(*j)++] = ' ';
    }
    else
  {
      result[(*j)++] = valeur[k++];
    }
  }
}

void  init_var(char *str, size_t	*i, size_t *j,size_t *old_size, bool *condition, bool *flag)
{
  *old_size = (ft_strlen(str) + 2 + 100);
  *i = 0;
  *j = 0;
  *condition = con(str);
  *flag = flaging(str);
}
