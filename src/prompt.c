#include "../include/minishell.h"

static char	*get_path_display(void)
{
	char	*pwd;
	char	*home;
	char	*path;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (NULL);
	home = getenv("HOME");
	if (home && ft_strnstr(pwd, home, ft_strlen(pwd)))
	{
		path = ft_strjoin("~", pwd + ft_strlen(home));
		free(pwd);
	}
	else
		path = pwd;
	return (path);
}

static char	*get_user_display(void)
{
	char	*user;
	char	*base;
	char	*full;

	user = getenv("USER");
	if (!user)
		user = "unknown";
	base = ft_strjoin("\033[3;46m", user);
	if (!base)
		return (NULL);
	full = ft_strjoin(base, "@shell\033[0m\033[1;33m | \033[1;32m");  // "\033[0m" = reset - "\033[1;33m" = bold yellow = "\033[1;32m" = bold green
	free(base);
	return (full);
}

char	*promt(void)
{
	char	*prompt;
	char	*path;
	char	*user;
	char	*p_arrow;

	path = get_path_display();
	if (!path)
		return (NULL);
	user = get_user_display();
	p_arrow = ft_strjoin(path, "\e[1;33m ➜  \033[0m");
	free(path);
	prompt = ft_strjoin(user, p_arrow);
	free(user);
	free(p_arrow);
	return (prompt);
}
