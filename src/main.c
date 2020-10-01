#include "minishell.h"
#include "utils.h"

/*
 ** Launch executable/program
 */

int	ft_pgrm(char *exe, char **args, char **env)
{
	pid_t	pid;

	pid = fork();
	errno = 0;
	if (pid < 0)
		strerror(errno);
	else if (!pid)
		execve(exe, args, env) == -1 ? strerror(errno) : 0;
	else
		wait(&pid);
	return (1);
}

/*
 ** Checking absence of lonely quotation mark (either ' (->sq) or " (-> dq))
 */

int	check_syntax(char *s, int sq, int dq, int i)
{
	while (!(s[i] == '\'' && !dq) && !(s[i] == '\"' && !sq) && s[i])
		i++;
	if (s[i] == '\'' && !dq)
		sq = 1 - sq;
	if (s[i] == '\"' && !sq)
		dq = 1 - dq;
	if (s[i])
		return (check_syntax(s + i, sq, dq, 1));
	return (sq + dq);
}

/*
 ** Analysis of cmds : 1. builtin? 2. executable? 3. other?
 */

char	**ft_run(char *cmd, char **env)
{
	char	**args;
	char	*tmp;
	int	i;

	if (!cmd)
		return (env);
	args = ft_split_inc(cmd, SPACE);
	i = -1;
	if (check_syntax(cmd, 0, 0, 0))
	{
		ft_putstr(strerror(WRONG_ARG));
		return (env);
	}
	free(cmd);
	cmd = NULL;
	tmp = ft_strrmv(args[0], SPACE);
	free(args[0]);
	args[0] = tmp;
	i = -1;
	while (++i < BLTS)
		if (!ft_strcmp(args[0], bt_str(i)))
			return ((*bt_ft(i))(args, env));
	if (args[0][0] == '.' && args[0][1] == '/')
		ft_pgrm(args[0], args, env);
	return (env);
}

/*
 ** Malloc and copy my own env
 ** Note : the function used is ft_envadd that copies the env and adds an element 
 ** (hence expt == NULL here)
 */


char	**ft_envadd(char **envp, char *expt)
{
	int	i;
	char	**env;

	i = 0;
	while (envp[i])
		i++;
	if (!(env = malloc(sizeof(char *) * (i + 1 + 1))))
		return (NULL);
	i = -1;
	while (envp[++i])
		env[i] = ft_strdup(envp[i]);
	if (expt != NULL)
		env[i++] = ft_strdup(expt);
	env[i] = NULL;
	return (env);
}

/*
 ** Read and tokenize cmd line based on ";"
 */

int	main(int ac, char **av, char **envp)
{
	int	status;
	char	**cmd;
	char	*line;
	int	j;
	char	**env;

	(void)ac;
	(void)av;
	display_ascii_dude();
	env = ft_envadd(envp, NULL);
	status = 1;
	while (status)
	{
		ft_putstr("> ");
		line = NULL;
		get_next_line(0, &line);
		cmd = ft_split_exc(line, ";");
		j = 0;
		while (cmd[j])
			env = ft_run(cmd[j++], env);
		status = env ? 1 : 0;
		free(cmd);
		free(line);
	}
	return (0);
}
