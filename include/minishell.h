#ifndef MINISHELL_H
# define MINISHELL_H

# ifndef COLOR_START
#  define COLOR_START "\001\033[1;36m\002"
# endif
# ifndef COLOR_RESET
#  define COLOR_RESET "\001\033[1;33m\002"
# endif
# include "../my_libft/libft.h"
# include <dirent.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <uchar.h>
# include <unistd.h>

extern int				g_value;

// Garbage Collector structures
typedef struct s_gc_node
{
	void				*ptr;
	struct s_gc_node	*next;
}						t_gc_node;

typedef struct s_gc
{
	t_gc_node			*allocations;
	size_t				count;
}						t_gc;

typedef enum s_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HERDOC
}						t_token_type;

// typedef struct s_info
// {
//   bool should_join;
// } t_info;

typedef struct s_token
{
	char				*av;
	bool				info;
	t_token_type		type;
	struct s_token		*next;
	struct s_token		*prev;
}						t_token;

typedef struct s_command
{
	char				**args;
	char				**file_input;
	char				*file_output;
	char				**herdoc;
	char				*herdoc_file;
	int					append;
	bool				file;
	struct s_command	*next;
}						t_command;

typedef struct s_data
{
	int					exit;
	int					end;
	int					start;
	bool				should_expand_outside;
	bool				should_expand_inside;
	int					count_herdoc;
	int					count_red_in;
}						t_data;

void					join_nodes(t_token **token);
void					excute_redirection_of_child_builtin(t_command **cmd,
							int *fd_out, t_data *data);
void					excute_redirection_of_parent(t_command **cmd,
							int *fd_out, t_data *data, int *fd1);
int						is_directory_parent(t_command **cmd);
void					open_and_duplicate(t_command **cmd, int *flags,
							int *fd_out);
int						is_number(char *str);
void					my_exit_child(t_command **cmd, t_data *data);
void					my_exit(t_command **cmd, t_data *data, int *error);
int						make_exit(t_command *cmd);
int						validation(t_command *cmd);
void					excute_redirection_of_child(t_command **cmd,
							t_data **data, int *fd_out, int *fd_in);
int						append_or_trunc(t_command **cmd);
int						is_directory(t_command **cmd);
void					open_red_out(t_command **cmd, int *fd_out);
void					open_red_in(t_data **data, int *fd_in, t_command **cmd);
int						heredoc_realloc(int *i, t_command *cmd,
							t_token **current);
int						red_in_realloc(t_command *cmd, t_data **data,
							t_token **current);
char					*make_content(char *line, t_data **data);
void					mix(bool *should_join, char *line, int i);
int						*init_status(void);
void					set_status(int val);
int						get_status(void);
void					my_handler(int sig);
void					cleanup_exit_handler(int sig);
void					free_array(char **arr);
void					my_echo(t_command *cmd);
void					my_exit_child(t_command **cmd, t_data *data);
void					excute_redirection_of_child_builtin(t_command **cmd,
							int *fd_out, t_data *data);
void					check_exit_status(t_command *cmd, t_data **data);
void					excute_herdoc_for_child(t_command **cmd, t_data **data);
bool					built_in(char *cmd);
void					free_2D_array(char **str);
char					*get_command(char *cmd, char **env);
void					execute_command(t_command *cmd, char **env,
							t_data **data);
void					handle_dollar(t_token **token, char *line,
							t_data **data);
void					handle_special_quot(t_token **token, char *line,
							t_data **data);
void					handle_white_spaces(t_token **token, char *line,
							t_data **data);
bool					check_somthing(char *word);
void					handle_some_cases(t_token **token, char *line,
							t_data **data);
char					*manual_realloc(char *old, size_t len);
int						handle_pipe(t_token **current, t_command **current_cmd,
							t_command *first_cmd, t_data **data);
int						handle_redir_in(t_token **current, t_command *cmd,
							t_data **data);
int						handle_redir_out(t_token **current, t_command *cmd);
int						handle_redir_append(t_token **current, t_command *cmd);
int						handle_heredoc(t_token **current, t_command *cmd,
							int *i);
bool					con(char *str);
bool					flaging(char *str);
void					make_like_bash(char *result, char *valeur, size_t *j);
void					init_var(char *str, size_t *i, size_t *j,
							size_t *old_size, bool *condition, bool *flag);
void					print_commands(t_command *cmd);
void					print_token(t_token *token);
bool					special_character(char *str);
char					*promt(void);
void					make_prompt(char **env);
bool					special_character(char *str);
t_token					*creat_token(char *line, t_token_type type,
							bool should_join);
void					handle_quote(bool *in_quot, char *quot_char, int *i,
							char *line);
void					add_token(t_token **token, t_token *new_token);
bool					is_closed_quotes(char *str);
void					handle_word_token(t_token **token, char *line,
							t_data **data);
bool					logic_of_meta(t_token *cmd, t_data **data);
t_token_type			get_token_type(char *str);
int						handle_speciale_token(t_token **token, char *line,
							int i, t_data **data);
t_token					*tokenize(char *line, t_data **data);
void					append_arg(t_command *cmd, char *str, t_data **data);
t_command				*create_command(void);
void					free_cmd(t_command *cmd);
char					*expand_env(char *str);
t_command				*parsing_command(t_token *token, t_data **data);
int						is_space(char c);
void					free_token(t_token **token);
char					*remove_quotes(char *str);

// Garbage Collector functions
t_gc					*gc_init(void);
void					*gc_malloc(size_t size);
char					*gc_strdup(const char *s);
void					*gc_calloc(size_t count, size_t size);
char					*gc_substr(char const *s, unsigned int start,
							size_t len);
char					*gc_strjoin(char const *s1, char const *s2);
void					gc_free(void *ptr);
void					gc_register_external(void *ptr);
void					gc_cleanup(void);
void					gc_cleanup_partial(void);
size_t					gc_get_allocation_count(void);
void					gc_print_allocations(void);

#endif
