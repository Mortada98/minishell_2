#include "../include/minishell.h"

int update_existing_var(char *name, char *value, char **env)
{
	int		i;
	size_t	len = ft_strlen(name);
	char	*new_entry;
	char	*tmp;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (0);
	new_entry = ft_strjoin(tmp, value);
	free(tmp);
	if (!new_entry)
		return (0);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
		{
			free(env[i]);
			env[i] = new_entry;
			return (1);
		}
		i++;
	}
	free(new_entry);
	return (0);
}

int my_export(char *arg, char ***env)
{
	char	*equal_sign;
	char	*name;
	char	*value;
	char	*new_var;
	char	*tmp;

	if (!arg || !*arg)
		return (print_env(*env), 1);
	equal_sign = ft_strchr(arg, '=');
	if (!equal_sign)
		return (1);
	name = ft_substr(arg, 0, equal_sign - arg);
	value = ft_strdup(equal_sign + 1);
	if (!name || !value)
		return (free(name), free(value), 0);
	if (!update_existing_var(name, value, *env))
	{
		tmp = ft_strjoin(name, "=");
		new_var = ft_strjoin(tmp, value);
		free(tmp);
		if (!new_var)
			return (free(name), free(value), 0);
		add_env_variable(new_var, env);
	}
	free(name);
	free(value);
	return (1);
}

int my_unset(char *name, char ***env)
{
	int		i = 0, j = 0;
	size_t	len = ft_strlen(name);
	char	**new_env;

	while ((*env)[i])
		i++;
	new_env = malloc(sizeof(char *) * i);
	if (!new_env)
		return (0);
	i = 0;
	while ((*env)[i])
	{
		if (!(ft_strncmp((*env)[i], name, len) == 0 && (*env)[i][len] == '='))
			new_env[j++] = (*env)[i];
		else
			free((*env)[i]);
		i++;
	}
	new_env[j] = NULL;
	free(*env);
	*env = new_env;
	return (1);
}

void	my_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		printf("%s\n", cwd);
		free(cwd);
	}
	else
	{
		perror("pwd");
		set_status(1);
	}
}
