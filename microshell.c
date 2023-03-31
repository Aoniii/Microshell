#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int	ft_strlen(char *str) {
	int	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_putstr(char *str) {
	write(2, str, ft_strlen(str));
}

char	**parser(int argc, char **argv, int *i) {
	char	**cmd;
	int		j;
	int		k;

	j = *i;
	while (*i < argc && ((argv[*i][0] != ';' && argv[*i][0] != '|') || ft_strlen(argv[*i])  > 1))
		*i += 1;
	cmd = malloc(sizeof(char *) * (*i - j + 1));
	if (!cmd)
		return (NULL);
	k = 0;
	while (j < *i) {
		cmd[k] = argv[j];
		j++;
		k++;
	}
	cmd[k] = NULL;
	return (cmd);
}

void builtin_cd(char **cmd) {
	if (cmd[1] && !cmd[2]) {
		if (chdir(cmd[1]) == -1) {
			ft_putstr("error: cd: cannot change directory to ");
			ft_putstr(cmd[1]);
			ft_putstr("\n");
		}
		return;
	}
	ft_putstr("error: cd: bad arguments\n");
}

void	fork_cmd(char **cmd, int *in, int *out, char **env) {
	int	pid;

	pid = fork();
	if (pid == -1)
		exit(EXIT_FAILURE);
	else if (pid == 0) {
		if (in[0] != -1) {
			dup2(in[0], 0);
			close(in[0]);
		}
		if (out[0] != -1) {
			dup2(out[1], 1);
			close(out[0]);
			close(out[1]);
		}
		execve(cmd[0], cmd, env);
		ft_putstr("error: cannot execute ");
		ft_putstr(cmd[0]);
		ft_putstr("\n");
	}
	if (in[0] != -1)
		close(in[0]);
	if (out[0] != -1)
		close(out[1]);
	in[0] = out[0];
	in[1] = out[1];
	waitpid(pid, NULL, 0);
}

int	main(int argc, char **argv, char **env) {
	char	**cmd;
	int		i;
	int		in[2];
	int		out[2];

	i = 1;
	in[0] = -1;
	out[0] = -1;
	while (i < argc) {
		cmd = parser(argc, argv, &i);
		if (!cmd)
			return (1);
		if (cmd[0]) {
			if (strcmp(cmd[0], "cd") == 0) {
				builtin_cd(cmd);
			} else if (i < argc && argv[i][0] == '|') {
				if (pipe(out) == -1) {
					ft_putstr("error: fatal\n");
					return (1);
				}
				fork_cmd(cmd, in, out, env);
			} else {
				fork_cmd(cmd, in, out, env);
				in[0] = -1;
				out[0] = -1;
			}
		}
		i++;
		free(cmd);
		cmd = NULL;
	}
	return (0);
}
