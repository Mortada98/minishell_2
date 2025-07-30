#include "../include/minishell.h"

void	compare_newline(char **str, bool *j, int *i)
{
	int	k;

	k = 0;
	while (str[*i] && ft_strncmp(str[*i], "-n", 2) == 0)
	{
		k = 1;
		while (str[*i][k])
		{
			if (str[*i][k] != 'n')
			{
				*j = false;
				return ;
			}
			k++;
		}
		*j = true;
		(*i)++;
	}

}

void	my_echo(t_command *cmd)
{
	int		i;
	bool	j;

	j = false;
	i = 1;
	if (!cmd->args[i])
	{
		printf("\n");
		return ;
	}
	compare_newline(cmd->args, &j, &i);
	while (cmd->args[i])
	{
		ft_putstr_fd(cmd->args[i], 1);
		if (cmd->args[i + 1])
			ft_putstr_fd(" ", 1);
		i++;
	}
	if (j == false)
		printf("\n");
}

bool	built_in(char *cmd)
{
	if (ft_strncmp(cmd, "exit", 4) == 0)
		return (true);
	if (ft_strncmp(cmd, "echo", 5) == 0)
		return (true);
	else
	{
		return (false);
	}
}

void	free_2D_array(char **str)
{
	(void)str;
}

char	*get_command(char *cmd, char **env)
{
	char	*path_env;
	char	**split_env;
	char	*complete_path;
	char	*first_join;
	int		i;
	DIR		*folder;

	i = 0;
	if (ft_strchr(cmd, '/'))
	{
		// Check if it's a directory first
		folder = opendir(cmd);
		if (folder != NULL)
		{
			printf("minishell: %s: Is a directory\n", cmd);
			set_status(126);
			closedir(folder);
			return (NULL);
		}
		closedir(folder);
		
		if (access(cmd, X_OK) == 0)
		{
			return (gc_strdup(cmd));
		}
		if (access(cmd, F_OK) == 0)
		{
			return (gc_strdup(cmd));
		}
		return (NULL);
	}
	while (env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
		{
			path_env = env[i] + 5;
			break ;
		}
		i++;
	}
	split_env = ft_split(path_env, ':');
	first_join = gc_strjoin("/", cmd);
	i = 0;
	while (split_env[i])
	{
		complete_path = gc_strjoin(split_env[i], first_join);
		if (access(complete_path, X_OK) == 0)
		{
			return (complete_path);
		}
		if (access	(complete_path, F_OK) == 0)
		{
			//printf("minishell: %s: Permission denied\n", complete_path);
			return (complete_path);
		}
		i++;
	}
	return (NULL);
}
