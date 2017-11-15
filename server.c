#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "ft_p.h"



typedef struct	s_cs
{
	int			fd;
	char		*home;
	char		*pwd;
	char		*oldpwd;
}	t_cs;

int	fork_process(void)
{
	pid_t 	pid;
	int		ret;
	int status;
	char **arg;

	arg = malloc(sizeof(char *)*2);
	*arg = strdup("ls");
	*(arg + 1) = NULL;
	if ((pid = fork()) == -1)
		return (1);
	if (!pid)
	{
		execv("/bin/ls", arg);
		exit(2);
	}
	else
	{
		wait(&status);
		ret = WEXITSTATUS(status);
	}
	return (0);
}

int	free_cs(t_cs *cs)
{
	if (cs->home)
		free(cs->home);
	if (cs->pwd)
		free(cs->pwd);
	if (cs->oldpwd)
		free(cs->oldpwd);
	return (EXIT_FAILLURE);
}

int	init_cs(t_cs *cs, int fd)
{
	if (!(cs->pwd = ft_strnew(PWD_MAX_LEN)))
		return (EXIT_FAILLURE);
	if (!(cs->pwd = getcwd(cs->pwd, PWD_MAX_LEN)))
		return (EXIT_FAILLURE);
	cs->fd = fd;
	cs->home = NULL;
	cs->oldpwd = NULL;
	if (!(cs->home = ft_strdup(cs->pwd)))
		return (free_cs(cs));
	if (!(cs->oldpwd = ft_strdup(cs->pwd)))
		return (free_cs(cs));
	return (EXIT_SUCCESS);
}

int	quit_requet(t_cs *cs)
{
	send_requet(
		cs->fd, R_QUIT, 0, NULL);
	return (QUIT);
}

int	execute_cd(t_cs *cs, char *pwd)
{
	char *buf;

	printf("pwd: %s\n", pwd);
	if (chdir(pwd))
		return (send_error(cs->fd, NO_ACCESS));
	if (!(buf = getcwd(NULL, RECV_SIZE)))
		return (send_error(cs->fd, INTERN_ERROR));
	free(cs->oldpwd);
	cs->oldpwd = cs->pwd;
	cs->pwd = buf;
	send_requet(
		cs->fd, R_SUCCESS, 0, NULL);
	return (EXIT_SUCCESS);
}

int	verify_dest(t_cs *cs, char *dest)
{
	char	**split;
	int		i;
	int		dir;

	if (!(split = ft_strsplit(dest, '/')))
		return (EXIT_FAILLURE);
	i = 0;
	dir = ft_nb_c(cs->pwd + ft_strlen(cs->home), '/');
	while (split[i])
	{
		if (dir < 0)
			break ;
		if (!ft_strcmp("..", split[i]))
			dir--;
		else
			dir++;
		i++;
	}
	ft_array_free(&split);
	if (dir < 0)
		return (EXIT_FAILLURE);
	return (EXIT_SUCCESS);
}

char	*new_pwd_target(t_cs *cs, char **requet)
{
	char	*target;

	if (*requet[1] != '/')
	{
		if (!(target = ft_strjoin(cs->pwd, "/")))
			return (NULL);
		if (!(target = ft_strjoin_free(&target, requet[1])))
			return (NULL);
	}
	else
	{
		if (!(target = ft_strjoin(cs->home, requet[1])))
			return (NULL);
	}
	return (target);
}

int	cd_requet(t_cs *cs, char **requet)
{
	int		ret;
	int		len;
	char	*target;

	if ((len = ft_array_len((const void **)requet)) < 2)
		return (execute_cd(cs, cs->home));
	else if (len > 2)
		return (send_error(cs->fd, TOO_MUCH_ARG));
	if (!ft_strcmp(requet[1], "-"))
		return (execute_cd(cs, cs->oldpwd));
	if (!(target = new_pwd_target(cs, requet)))
		return (send_error(cs->fd, INTERN_ERROR));
	if (verify_dest(cs, requet[1]) == EXIT_FAILLURE)
	{
		free(target);
		return (send_error(cs->fd, NO_ACCESS));
	}
	ret = execute_cd(cs, target);
	free(target);
	return (ret);
}

int	send_data(int fd, void *src, size_t size)
{
	int len;
	void *ptr_end;

	ptr_end = (void *)src + size;
	len = 0;
	while (src < ptr_end)
	{
		len = RECV_SIZE;
		if ((ptr_end - src) < RECV_SIZE)
		{
			len = ptr_end - src;
		}
		if (send(fd, src, len, 0) == C_LOST)
			return (C_LOST);
		src = src + len;
	}
	return (EXIT_SUCCESS);
}

int send_data_by_size(int fd, void *data, size_t size)
{
	if (send_requet(
		fd, R_WAIT_SEND, size, NULL) == C_LOST)
		return (C_LOST);
	if (wait_reponse(fd, R_WAIT_RECV, -1, NO_LOG) < 0)
		return (EXIT_FAILLURE);

	if (send_data(fd, data, size) == C_LOST)
		return (C_LOST);

	if (wait_reponse(fd, R_RECV, size, NO_LOG) < 0)
		return (send_error(fd, TRANSFERT_FAIL));
	return (send_success(fd));
}

int	pwd_requet(t_cs *cs, char **requet)
{
	size_t		size;
	char		*data;
	int			ref;

	if (ft_array_len((const void **)requet) > 1)
		return (send_error(cs->fd, TOO_MUCH_ARG));
	if (!ft_strcmp(cs->home, cs->pwd))
	{
		size = 1;
		ref = 0;
	}
	else
	{
		ref = ft_strlen(cs->home);
		size = ft_strlen(cs->pwd) - ref;
	}
	return (send_data_by_size(cs->fd, (cs->pwd + ref), size));
}

int	switch_requet(t_cs *cs, char *requet)
{
	int			ret;
	char		**split;

	ret = MAGIC_NUMER;
	if (!(split = ft_strsplit(requet, ' ')))
	{
		if ((ret = send_requet(
			cs->fd, R_ERROR, ft_strlen(INTERN_ERROR), INTERN_ERROR)))
			return (ret);
		return (EXIT_FAILLURE);
	}
	if (!ft_strncmp(requet, REQUET_QUIT, ft_strlen(REQUET_QUIT)))
		ret = quit_requet(cs);
	else if (!ft_strncmp(requet, REQUET_CD, ft_strlen(REQUET_CD)))
		ret =  cd_requet(cs, split);
	else if (!ft_strncmp(requet, REQUET_PWD, ft_strlen(REQUET_CD)))
		ret =  pwd_requet(cs, split);
	ft_array_free(&split);
	if (ret == MAGIC_NUMER)
	{
		if ((ret = send_requet(
			cs->fd, R_ERROR, ft_strlen(UNKNOW_CMD), UNKNOW_CMD)))
			return (ret);
		return (EXIT_SUCCESS);
	}
	return (ret);
}

int	read_requet(t_cs *cs, void *buf, int read)
{
	t_header *header;

	header = (t_header *)buf;
	if (header->requet != R_CMD
		|| !header->size || (header->size + sizeof(header)) != read)
	{
		if (send_requet(cs->fd, R_ERROR,
			ft_strlen(NEED_COMMAND_VALID), NEED_COMMAND_VALID) == C_LOST)
			return (C_LOST);
		return (EXIT_SUCCESS);
	}
	printf("code: %d, size: %d, data: %s\n", header->requet, header->size, (void *)header + sizeof(header));
	return (switch_requet(cs, (void *)header + sizeof(header)));
}

int	read_socket(int fd)
{
	t_cs	cs;
	char	*buf;
	int	read;
	int		ret;

	if (init_cs(&cs, fd))
		return (EXIT_FAILLURE);
	if (!(buf = ft_strnew(RECV_SIZE)))
		return (EXIT_FAILLURE);
	ft_bzero(buf, RECV_SIZE);
	ret = 0;
	while ((read = recv(fd, buf, RECV_SIZE, 0)) > 0)
	{
		if (read >= (int)SIZE_HEADER)
		{
			if ((ret = read_requet(&cs, buf, read)))
				if (ret == QUIT || ret == C_LOST)
					break ;
		}
		ft_bzero(buf, RECV_SIZE);
	}
	free(buf);
	free_cs(&cs);
	return (ret);
}

int	new_process(int fd)
{
	pid_t	pid;
	int		ret;

	printf("%s\n", "new process");
	if ((pid = fork()) == -1)
		return (EXIT_FAILLURE);
	if (!pid)
	{
		ret = read_socket(fd);
		close(fd);
	}
	return (EXIT_SUCCESS);
}

/*
** usage
*/

void	usage(char *s)
{
	printf("Usage: %s <port>\n", s);
	exit(EXIT_FAILLURE);
}

int	create_server(int port)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;

	if (!(proto = getprotobyname(PROTOCOLE)))
		return (-1);
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sock, (const struct sockaddr *)&sin, sizeof(sin));
	listen(sock, NB_CONN_SOCKET);
	return (sock);
}

/*
** TCP/IP (v4) server example from 42 school
*/

int	main(int ac, char **av)
{
	int					port;
	int					sock;
	int					cs;
	unsigned int		cslen;
	struct sockaddr_in	csin;
	int					nb_con;

	if (ac != 2)
		usage(av[0]);
	if ((port = atoi(av[1])) <= 0)
		usage(av[0]);
	if ((sock = create_server(port)) < 0)
	 	return (EXIT_FAILLURE);
		printf("%s\n", "yo");
	while (42)
	{
		if ((cs = accept(sock, (struct sockaddr *)&csin, &cslen)) == -1)
		{
			ft_putstr_fd(MESS_LIMIT_NB_CON_REACHED, STDERR);
			continue ;
		}
		new_process(cs);
	}



	close(cs);
	close(sock);
	return (EXIT_SUCCESS);
}
