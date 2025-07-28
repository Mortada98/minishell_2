#include "../include/minishell.h"
#include <stdio.h>

int g_value = 0;
static void  read_and_convert(char *buffer, int *fd, unsigned char *c, int *i)
{
  if (read(*fd, c, 1) != 1)
  {
    close(*fd);
    return;
  }
  buffer[(*i)++] = 'a' + (*c % 26);
}

static char *generate_file_name()
{
  unsigned char c;
  char *buffer; 
  int fd, i;
  static int counter = 0;

  buffer = gc_malloc(11);
  if (!buffer)
    return NULL;
  
  // Try /dev/urandom first, then /dev/random, then use a counter fallback
  fd = open("/dev/urandom", O_RDONLY);
  if (fd < 0)
    fd = open("/dev/random", O_RDONLY);
  
  if (fd < 0)
  {
    // Fallback: use a counter-based approach
    snprintf(buffer, 11, "hd_%06d", counter++);
    return buffer;
  }
  
  i = 0;
  while(i < 10)
    read_and_convert(buffer, &fd, &c, &i);
  buffer[i] = '\0';
  close(fd);
  return (buffer);
}

static void  make_loop(t_command **cmd , int *fd, int i, t_data **data)
{
  char *line;
  char *str;
  char *expanded_line;

  g_value = 0;
  while(1)
  {
    line = readline("> ");
    if (!line)
    {
      if (!g_value)
        printf("warning: here-document at line delimited by end-of-file (wanted `%s')\n", (*cmd)->herdoc[i]);
      break;
    }
    
    // Register readline's allocation with GC
    gc_register_external(line);
    
    if (!(*cmd)->herdoc || (*cmd)->herdoc[i] == NULL)
    {
      return;
    }
    if (strcmp(line, (*cmd)->herdoc[i]) == 0)
    {
      return;
    }
    str = line;
    if (!(*data)->should_expand_inside)
    {
      expanded_line = expand_env(str);
      if (expanded_line && expanded_line != str)
      {
        write(*fd, expanded_line, ft_strlen(expanded_line));
        // expand_env uses GC allocation, so no manual free needed
      }
      else
      {
        write(*fd, line, ft_strlen(line));
      }
    }
    else
    {
      write(*fd, line, ft_strlen(line));
    }
    write(*fd, "\n", 1);
  }
}

static void  minishell_init(char **buffer, char **join, int *fd)
{
    *buffer = generate_file_name();
    if (!*buffer)
    {
      *fd = -1;
      return;
    }
    *join = gc_strjoin("/tmp/", *buffer);
    if (!*join)
    {
      *buffer = NULL;
      *fd = -1;
      return;
    }
    *fd = open(*join, O_RDWR | O_CREAT, 0644);
    if (*fd < 0)
    {
      *buffer = NULL;
      *join = NULL;
      return;
    }
}

static void  my_server(int ig)
{
  if (ig == SIGINT)
  {
    close (0);
    g_value = 1;
    write(1, "\n", 1);
  }
}

int  herdoc_condition_2(t_command **cmd, t_data **data)
{
  (void)data;
  if (g_value == 1)
  {
    (*cmd)->file = true;
    set_status(130);
    return(0);
  }
  return(1);
}

void  herdoc_condition_1(t_command **cmd, t_data **data, char *join, int i)
{
  if (i == (*data)->count_herdoc - 1)
    (*cmd)->herdoc_file = gc_strdup(join);
  else
    unlink(join);
}

void excute_herdoc_for_child(t_command **cmd, t_data **data)
{
  int i, fd;
  char *buffer, *join;

  if (!cmd || !*cmd || !data || !*data)
    return;

  i = 0;
  int save = dup(0);
  while (i < (*data)->count_herdoc)
  {
    minishell_init(&buffer, &join, &fd);
    if (!buffer || !join || fd < 0)
    {
      break;
    }
    signal(SIGINT, my_server);
    make_loop(cmd, &fd, i, data);
    close(fd);
    herdoc_condition_1(cmd, data, join, i);
    if (!herdoc_condition_2(cmd, data))
      break;
    i++;
  }
  dup2(save, 0);
  close(save);
  // Restore the main shell signal handler but don't reset g_value yet
  // g_value will be reset by the caller after all heredocs are processed
  signal(SIGINT, my_handler);
  (*data)->count_herdoc = 0;
  (*data)->should_expand_inside = false;
}
