#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "ft_p.h"


int	save_std(int fd1_dst, int fd1_src, int fd2_dst, int fd2_src)
{
	if (dup2(fd1_dst, fd1_src) == -1 || dup2(fd2_dst, fd2_src) == -1)
		return (EXIT_FAILLURE);
	return (EXIT_SUCCESS);
}


int	fork_process_cmd(int fd, char **arg)
{
	pid_t 	pid;
	int		ret;
	int status;

	if ((pid = fork()) == -1)
		return (-1);
	if (!pid)
	{
		if (save_std(fd, STDOUT, fd, STDERR))
			return (send_error(fd, INTERN_ERROR));
		execv("/bin/ls", arg);
		exit(-1);
	}
	else
	{
		wait(&status);
		ret = WEXITSTATUS(status);
		return (ret);
	}
	return (EXIT_SUCCESS);
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

int	put_requet(t_cs *cs, char **requet, char *requet_s)
{
	int			size;
	char		*buf;
	int 		ret;

	if (ft_array_len((const void **)requet) != 2)
		return (send_error(cs->fd, INVALID_NB_ARG));
	if (send_requet(
		cs->fd, R_PUT_OK, 0, NULL) == C_LOST)
	{
			return (C_LOST);
	}
	if (get_reponse(cs->fd, requet_s))
	{
		return (send_error(cs->fd, TRANSFERT_FAIL));
	}
	return (send_success(cs->fd));
	// return (EXIT_SUCCESS);
}

int	verify_multi_dest(t_cs *cs, char **requet)
{
	int i;

	i = 0;
	if (!*requet)
		return (EXIT_SUCCESS);
	while (requet[i] && *requet[i] == '-')
		i++;
	while (requet[i])
	{
		if (verify_dest(cs, requet[i]))
			return (EXIT_FAILLURE);
		i++;
	}
	return (EXIT_SUCCESS);
}

int	exec_cmd(int fd, char **requet)
{
	int 	ret;
	char	*end;

	if (!(end = ft_strnew(1)))
		return (send_error(fd, INTERN_ERROR));
	ft_bzero(end, 1);
	ret = fork_process_cmd(fd, requet);
	end[0] = EOT;
	ret = send(fd, end, 1, 0);
	free(end);
	if ((ret = wait_reponse(fd, R_SUCCESS, -1, IS_LOG)))
		return (ret);
	// return (ret < 0 ? C_LOST : EXIT_SUCCESS);
	return (ret < 0 ? send_error(fd, INTERN_ERROR) : send_success(fd));
}



int	cmd_requet(t_cs *cs, char **requet)
{
	int	ret;

	printf("requete : %s\n", *requet);

	if (verify_multi_dest(cs, requet + 1))
		return (send_error(cs->fd, NO_ACCESS));
	if ((ret = send_requet(
		cs->fd, R_CMD_OK, 0, NULL)))
		return (ret);
	if (wait_reponse(cs->fd, R_WAIT_RECV, -1, NO_LOG) < 0)
		return (send_error(cs->fd, INTERN_ERROR));

	printf("requete : %s\n", *requet);
	ret = exec_cmd(cs->fd, requet);
	// if (save_std(STDOUT, STDOUT, STDERR, STDERR))
	// 	return (EXIT_FAILLURE);
	return (ret);
}


int	get_requet_server(t_cs *cs, char **requet)
{
	if (ft_array_len((const void **)requet) != 2)
		return (send_error(cs->fd, INVALID_NB_ARG));
	if (verify_dest(cs, requet[1]) == EXIT_FAILLURE)
		return (send_error(cs->fd, NO_ACCESS));
	return (get_requet(cs->fd, requet, NO_LOG));
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
	else if (!ft_strncmp(requet, REQUET_PWD, ft_strlen(REQUET_PWD)))
		ret =  pwd_requet(cs, split);
	else if (!ft_strncmp(requet, REQUET_LS, ft_strlen(REQUET_LS)))
		ret =  cmd_requet(cs, split);
	else if (!ft_strncmp(requet, REQUET_GET, ft_strlen(REQUET_GET)))
		ret =  get_requet_server(cs, split);
	else if (!ft_strncmp(requet, REQUET_PUT, ft_strlen(REQUET_PUT)))
		ret =  put_requet(cs, split, requet);

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
