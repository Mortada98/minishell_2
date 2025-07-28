/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:26:13 by helfatih          #+#    #+#             */
/*   Updated: 2025/07/28 15:58:13 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	is_number(char *str)
{
	int	i;

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

int	validation(t_command *cmd)
{
	if (cmd->args[1] && !is_number(cmd->args[1]))
	{
		printf("minishell: exit: %s: numeric argument required\n",
			cmd->args[1]);
		return (2);
	}
	if (cmd->args[2])
	{
		if (!is_number(cmd->args[2]))
		{
			printf("minishell: exit: too many arguments\n");
			return (2);
		}
		else
		{
			printf("minishell: exit: too many arguments\n");
			return (1);
		}
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

int	append_or_trunc(t_command **cmd)
{
	if ((*cmd)->append)
	{
		return (O_APPEND);
	}
	else
		return (O_TRUNC);
}

int	is_directory(t_command **cmd)
{
	DIR	*folder;

	folder = opendir((*cmd)->file_output);
	if (folder != NULL)
	{
		printf("minishell : %s:  Is a directory\n", (*cmd)->file_output);
		set_status(1);
		closedir(folder);
		return (1);
	}
	closedir(folder);
	return (0);
}

void	excute_redirection_of_child(t_command **cmd, t_data **data, int *fd_out,
		int *fd_in)
{
	int	flags;
	int	i;

	flags = 0;
	i = 0;
	if ((*cmd)->file_input)
	{
		while (i < (*data)->count_red_in)
		{
			*fd_in = open((*cmd)->file_input[i], O_RDONLY);
			if (*fd_in < 0)
			{
				printf("minishell: %s: No such file or directory\n",
					(*cmd)->file_input[i]);
				set_status(1);
				exit(1);
			}
			dup2(*fd_in, STDIN_FILENO);
			close(*fd_in);
			i++;
		}
	}
	if ((*cmd)->file_output)
	{
		if (is_directory(cmd))
			exit(1);
		flags = O_WRONLY | O_CREAT | append_or_trunc(cmd);
		*fd_out = open((*cmd)->file_output, flags, 0644);
		if (*fd_out < 0)
		{
			perror("minishell");
			set_status(1);
			exit(1);
		}
		dup2(*fd_out, STDOUT_FILENO);
		close(*fd_out);
	}
}

void	my_exit(t_command **cmd, t_data *data, int *error)
{
	int	i;

	(void)data;
	if (strcmp((*cmd)->args[0], "exit") == 0)
	{
		printf("exit\n");
		i = make_exit(*cmd);
		gc_cleanup();
		rl_clear_history();
		if (i == 0)
		{
			set_status(get_status());
			*error = 1;
			return;
			//exit(get_status());
		}
		set_status(i);
		*error = 1;
		return;
		// exit(i);
	}
}

void	my_exit_child(t_command **cmd, t_data *data)
{
	int	i;

	(void)data;
	if (strcmp((*cmd)->args[0], "exit") == 0)
	{
		i = make_exit(*cmd);
		gc_cleanup();
		// if (data) free(data);
		rl_clear_history();
		if (i == 0)
		{
			set_status(get_status());
			exit(get_status());
		}
		set_status(i);
		exit(i);
	}
}

void	open_and_duplicate(t_command **cmd, int *flags, int *fd_out)
{
	if ((*cmd)->file_output)
	{
		*flags = O_WRONLY | O_CREAT | append_or_trunc(cmd);
		*fd_out = open((*cmd)->file_output, *flags, 0644);
		if (*fd_out < 0)
			return ;
		if (dup2(*fd_out, STDOUT_FILENO) == -1)
		{
			close(*fd_out);
			return ;
		}
		close(*fd_out);
	}
}

int	is_directory_parent(t_command **cmd)
{
	DIR	*folder;

	folder = opendir((*cmd)->file_output);
	if (folder != NULL)
	{
		printf("minishell : %s:  Is a directory\n", (*cmd)->file_output);
		set_status(1);
		closedir(folder);
		return (1);
	}
	closedir(folder);
	return (0);
}

void	excute_redirection_of_parent(t_command **cmd, int *fd_out, t_data *data, int *fd1)
{
	int(saved_stdout), saved_stdin, flags;
	int error = 0;
	if ((*cmd)->file_output)
	{
		if (is_directory_parent(cmd))
			return ;
	}
	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	open_and_duplicate(cmd, &flags, fd_out);
	my_exit(cmd, data, &error);
	if (error == 1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdout);
		close(saved_stdin);
		close(*fd1);
		exit(get_status());
	}
	my_echo(*cmd);
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin);
}

void	excute_redirection_of_child_builtin(t_command **cmd, int *fd_out,
		t_data *data)
{
	int(saved_stdout), saved_stdin, flags;
	if ((*cmd)->file_output)
	{
		if (is_directory_parent(cmd))
			return ;
	}
	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	open_and_duplicate(cmd, &flags, fd_out);
	my_exit_child(cmd, data);
	my_echo(*cmd);
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin);
}

int	*init_status(void)
{
	static int	var = 0;

	return (&var);
}

void	set_status(int val)
{
	int	*status;

	status = init_status();
	*status = val;
}

int	get_status(void)
{
	return (*init_status());
}

void	execute_command(t_command *cmd, char **env, t_data **data)
{
	int			prev_fd;
	int			fd[2];
	pid_t		pid;
	int			fd_out;
	int			fd_in;
	t_command	*curr;
	int			status;
	int			saved_stdin;
	pid_t		pids[1024];
	int			pid_count;
	t_command	*temp_cmd;
	int			save;
	int			hd_fd;
	char		*command;
	int			sig;
	bool		has_command;

	fd_out = -1;
	prev_fd = -1;
	fd_out = -1, fd_in = -1;
	curr = cmd;
	saved_stdin = dup(0);
	pid_count = 0;
	if (!cmd)
	{
		close(saved_stdin);
		return ;
	}
	temp_cmd = cmd;
	while (temp_cmd)
	{
		if (temp_cmd->file_input)
		{
			for (int i = 0; temp_cmd->file_input[i]; i++)
			{
				if (access(temp_cmd->file_input[i], R_OK) != 0)
				{
					printf("bash: %s: No such file or directory\n",
						temp_cmd->file_input[i]);
					set_status(1);
					dup2(saved_stdin, 0);
					close(saved_stdin);
					return ;
				}
			}
		}
		temp_cmd = temp_cmd->next;
	}
	if (cmd && cmd->next == NULL && cmd->args && cmd->args[0]
		&& built_in(cmd->args[0]))
	{
		excute_redirection_of_parent(&cmd, &fd_out, *data, &saved_stdin);
		dup2(saved_stdin, 0);
		close(saved_stdin);
		return ;
	}
	
	// Check if we have any valid commands before entering the pipeline
	temp_cmd = cmd;
	has_command = false;
	while (temp_cmd)
	{
		if (temp_cmd->args && temp_cmd->args[0] && temp_cmd->args[0][0] != '\0')
		{
			has_command = true;
			break;
		}
		temp_cmd = temp_cmd->next;
	}
	
	if (!has_command)
	{
		dup2(saved_stdin, 0);
		close(saved_stdin);
		return;
	}

	while (curr && !(cmd)->file)
	{
		save = dup(0);
		if (curr->next)
		{
			if (pipe(fd) == -1)
			{
				perror("pipe");
				set_status(1);
				close(save);
				dup2(saved_stdin, 0);
				close(saved_stdin);
				return ;
			}
		}
		pid = fork();
		if (pid == 0)
		{
			if ((cmd)->file)
			{
				(cmd)->file = false;
				set_status(130);
				exit(130);
			}
			signal(SIGQUIT, SIG_DFL);
			signal(SIGINT, SIG_DFL);
			if (prev_fd != -1)
			{
				dup2(prev_fd, 0);
				close(prev_fd);
			}
			if (!(curr->file) && curr->herdoc_file)
			{
				hd_fd = open(curr->herdoc_file, O_RDONLY);
				if (hd_fd < 0)
				{
					printf("minishell: No such file or directory\n");
					exit(1);
				}
				dup2(hd_fd, 0);
				close(hd_fd);
			}
			if (curr->next)
			{
				close(fd[0]);
				dup2(fd[1], 1);
				close(fd[1]);
			}
			excute_redirection_of_child(&curr, data, &fd_out, &fd_in);
			if (curr->args && curr->args[0])
			{
				if (built_in(curr->args[0]))
				{
					excute_redirection_of_child_builtin(&curr, &fd_out, *data);
					exit((*data)->exit);
				}
				command = get_command(curr->args[0], env);
				if (!command)
				{
					close(saved_stdin);
					close(save);
					fprintf(stderr, "minishell: %s: command not found\n",
						curr->args[0]);
					gc_cleanup();
					set_status(127);
					exit(127);
				}
				if (execve(command, curr->args, env) != 0)
				{
					close(saved_stdin);
					close(save);
					gc_cleanup();
					perror("execve");
					exit(1);
				}
				exit(1);
			}
			else
			{
				exit(0);
			}
		}
		else if (pid > 0)
		{
			pids[pid_count++] = pid;
			if (prev_fd != -1)
				close(prev_fd);
			if (curr->next)
			{
				close(fd[1]);
				prev_fd = fd[0];
			}
		}
		else
		{
			perror("fork");
			set_status(1);
			close(save);
			dup2(saved_stdin, 0);
			close(saved_stdin);
			return ;
		}
		dup2(save, 0);
		close(save);
		curr = curr->next;
	}
	signal(SIGINT, SIG_IGN);
	for (int i = 0; i < pid_count; i++)
	{
		waitpid(pids[i], &status, 0);
		if (WIFSIGNALED(status))
		{
			sig = WTERMSIG(status);
			if (sig == SIGINT)
				write(1, "\n", 1);
			else if (sig == SIGQUIT)
				write(2, "Quit (core dumped)\n", 19);
			set_status(128 + sig);
			(*data)->exit = 128 + WTERMSIG(status);
		}
		else if (i == pid_count - 1)
		{
			set_status(WEXITSTATUS(status));
		}
	}
	signal(SIGINT, my_handler);
	dup2(saved_stdin, 0);
	close(saved_stdin);
}

t_token_type	get_token_type(char *str)
{
	if (ft_strncmp(str, ">>", 2) == 0)
		return (TOKEN_REDIR_APPEND);
	else if (ft_strncmp(str, "<<", 2) == 0)
		return (TOKEN_HERDOC);
	else if (ft_strncmp(str, ">", 1) == 0)
		return (TOKEN_REDIR_OUT);
	else if (ft_strncmp(str, "<", 1) == 0)
		return (TOKEN_REDIR_IN);
	else if (ft_strncmp(str, "|", 1) == 0)
		return (TOKEN_PIPE);
	else
		return (TOKEN_WORD);
}

t_token	*tokenize(char *line, t_data **data)
{
	t_token	*token;

	token = NULL;
	(*data)->start = 0;
	(*data)->end = 0;
	if (!check_somthing(line))
		return (NULL);
	while (line[(*data)->end])
	{
		if (line[(*data)->end] == '$' && (ft_isalnum(line[(*data)->end + 1])
				|| line[(*data)->end + 1] == '_'))
		{
			handle_dollar(&token, line, data);
			continue ;
		}
		if (line[(*data)->end] == '$' && !ft_isalnum(line[(*data)->end + 1]))
		{
			handle_some_cases(&token, line, data);
		}
		if (line[(*data)->end] == '|' || line[(*data)->end] == '<'
			|| line[(*data)->end] == '>')
		{
			handle_word_token(&token, line, data);
			(*data)->end = handle_speciale_token(&token, line, (*data)->end,
					data);
			(*data)->start = (*data)->end;
		}
		else if (line[(*data)->end] == '\"' || line[(*data)->end] == '\'')
			handle_special_quot(&token, line, data);
		else if (line[(*data)->end] == ' ' || line[(*data)->end] == '\t')
			handle_white_spaces(&token, line, data);
		else
			(*data)->end++;
	}
	handle_word_token(&token, line, data);
	return (token);
}

void	join_nodes(t_token **token)
{
	char	*joined;

	t_token(*curr), *next;
	curr = *token;
	while (curr && curr->next)
	{
		if (curr->info)
		{
			next = curr->next;
			joined = gc_strjoin(curr->av, next->av);
			if (!joined)
			{
				printf("minishell: memory allocation failed\n");
				return ;
			}
			curr->av = joined;
			curr->next = next->next;
			if (!next->info)
				curr->info = false;
			continue ;
		}
		curr = curr->next;
	}
}

bool	logic_of_meta(t_token *cmd, t_data **data)
{
	t_token	*cur;

	(void)data;
	cur = cmd;
	while (cur && cur->next)
	{
		if (cur->type != TOKEN_PIPE && cur->type != TOKEN_WORD
			&& cur->next->type != TOKEN_PIPE && cur->next->type != TOKEN_WORD)
		{
			printf("minishell: syntax error  near unexpected token %s\n",
				cur->next->av);
			set_status(2);
			return (false);
		}
		else
			cur = cur->next;
	}
	return (true);
}

void	my_handler(int sig)
{
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		set_status(130);
	}
}

void	make_prompt(char **env)
{
	char		*line;
	t_token		*token;
	t_command	*cmd;
	t_data		*data;
	t_command	*temp_cmd;
	bool		heredoc_interrupted;
	int			herdoc_count;
	t_command	*check_cmd;
	bool		has_command;

	signal(SIGINT, my_handler);
	signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		data = malloc(sizeof(t_data));
		gc_register_external(data);
		if (!data)
		{
			printf("minishell: memory allocation failed\n");
			return ;
		}
		ft_memset(data, 0, sizeof(t_data));
		line = readline(promt());
		if (!line)
		{
			printf("exit\n");
			gc_cleanup();
			// gc_cleanup_partial();
			// free(data);
			return ;
		}
		gc_register_external(line);
		if (line[0] == '\0')
		{
			continue ;
		}
		if (line[0] != '\0')
		{
			add_history(line);
			token = tokenize(line, &data);
			if (!token)
			{
				continue ;
			}
			join_nodes(&token);
			if (logic_of_meta(token, &data) == false)
			{
				continue ;
			}
			cmd = parsing_command(token, &data);
			if (!cmd)
				continue ;
			temp_cmd = cmd;
			heredoc_interrupted = false;
			while (temp_cmd && !heredoc_interrupted)
			{
				if (temp_cmd->herdoc && temp_cmd->herdoc[0])
				{
					herdoc_count = 0;
					while (temp_cmd->herdoc[herdoc_count])
						herdoc_count++;
					data->count_herdoc = herdoc_count;
					excute_herdoc_for_child(&temp_cmd, &data);
					if (g_value == 1)
					{
						heredoc_interrupted = true;
						break ;
					}
				}
				temp_cmd = temp_cmd->next;
			}
			if (heredoc_interrupted)
			{
				g_value = 0;
				gc_cleanup_partial();
				continue ;
			}
			g_value = 0;
			if (cmd)
			{
				check_cmd = cmd;
				has_command = false;
				while (check_cmd)
				{
					if (check_cmd->args && check_cmd->args[0])
					{
						has_command = true;
						break ;
					}
					check_cmd = check_cmd->next;
				}
				if (has_command)
				{
					execute_command(cmd, env, &data);
				}
				else if (cmd->herdoc_file)
				{
					unlink(cmd->herdoc_file);
				}
			}
			gc_cleanup_partial();
		}
	}
	gc_cleanup();
	// free(data);
	rl_clear_history();
}

int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	make_prompt(env);
}
