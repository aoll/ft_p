
#include "ft_p.h"

#define EXIT_FAILLURE	1
#define EXIT_SUCCESS	0
#define PROTOCOLE		"tcp"
#define NB_CONN_SOCKET	42

/*
** usage
*/

void	usage(char *s)
{
	printf("Usage: %s <port>\n", s);
	exit(EXIT_FAILLURE);
}

int	create_client(char *addr, int port)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;

	if (!(proto = getprotobyname(PROTOCOLE)))
		return (-1);
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(addr);
	if (connect(sock, (const struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		printf("%s\n", "Connect error");
		exit(EXIT_FAILLURE);
	}
	return (sock);
}

int quit_requet_client(int fd, char *requet)
{
	send_requet(fd, R_CMD, ft_strlen(requet), (const void *)requet);
	ft_putstr(SUCCESS);
	return (QUIT);
}

int cd_requet_client(int fd, char *requet)
{
	char		*buf;
	int			ret;

	if ((ret = send_requet(fd, R_CMD, ft_strlen(requet),
	(const void *)requet)))
		return (ret);
	if (!(buf = ft_strnew(RECV_SIZE)))
		return (EXIT_FAILLURE);
	ret = recv(fd, buf, RECV_SIZE, 0);
	if (ret >= (int)SIZE_HEADER)
		print_reponse(buf);
	free(buf);
	return (ret <= 0 ? C_LOST : EXIT_SUCCESS);
}

int	requet_client(int fd, char *requet, int output)
{
	if (send_requet(fd, R_CMD, ft_strlen(requet),
	(const void *)requet))
		return (C_LOST);
	return (recv_by_size(fd, output));
}

int	read_result_cmd(int fd, int output)
{
	char	*buf;
	int		ret;

	ret = 0;
	if (!(buf = ft_strnew(RECV_SIZE)))
		return (EXIT_FAILLURE);
	ft_bzero(buf, RECV_SIZE);
	while ((ret = recv(fd, buf, RECV_SIZE, 0)) > 0)
	{
		// ft_putstr_fd(buf, output);
		if (buf[ret - 1] == EOT)
		{
			// printf("break ret : %d, buf[ret - 1] : %d, buf: %s\n", ret, buf[ret - 1], buf);
			// ft_putstr_fd(buf, output);
			write(output, buf, ret - 1);
			break;
		}
		// printf("ret : %d, buf[ret - 1] : %d, buf: %s\n", ret, buf[ret - 1], buf);
		write(output, buf, ret);
		ft_bzero(buf, RECV_SIZE);
	}
	// exit(0);
	free(buf);
	if (ret == C_LOST)
		return (C_LOST);
	send_success(fd);
	// ft_putstr(SUCCESS);
	// return (EXIT_SUCCESS);
	// if (ret == C_LOST || send_success(fd))
	// 	return (C_LOST);
	return (wait_reponse(fd, R_SUCCESS, -1, IS_LOG));
}

int	requet_cmd(int fd, char *requet, int output)
{
	int ret;

	if (send_requet(fd, R_CMD, ft_strlen(requet),
	(const void *)requet))
		return (C_LOST);
	if ((ret = wait_reponse(fd, R_CMD_OK, -1, IS_LOG)))
		return (ret);
	if (send_requet(fd, R_WAIT_RECV, 0, NULL))
			return (C_LOST);
	return (read_result_cmd(fd, output));
	// return (recv_by_size(fd, output));
}


int	requet_get(int fd, char *requet)
{
	if (send_requet(fd, R_CMD, ft_strlen(requet),
		(const void *)requet) == C_LOST)
		return (C_LOST);
	return (get_reponse(fd, requet));
}

int check_file(char **split)
{
	int			fd;
	char		*ptr;

	if (!*(split + 1) || *(split + 2))
	{
		printf("ERROR\n%s", INVALID_NB_ARG);
		return (EXIT_FAILLURE);
	}
	if ((fd = open(*(split + 1), O_RDONLY)) < 0)
	{
		printf("ERROR\n%s", NO_ACCESS);
		return (EXIT_FAILLURE);
	}
	close(fd);
	return (EXIT_SUCCESS);
}


int	requet_put(int fd, char *requet)
{
	int		ret;
	char	**split;

	if (!(split = ft_strsplit(requet, ' ')))
		return (EXIT_FAILLURE);
	if (check_file(split))
		return (EXIT_FAILLURE);
	if (send_requet(fd, R_CMD, ft_strlen(requet),
		(const void *)requet) == C_LOST)
		return (C_LOST);
	if ((ret = wait_reponse(fd, R_PUT_OK, -1, IS_LOG)) < 0)
	{
		if (ret == C_LOST)
			return (C_LOST);
		return (EXIT_FAILLURE);
	}
	ret = get_requet(fd, split, IS_LOG);
	if ((ret = wait_reponse(fd, R_SUCCESS, -1, IS_LOG)) < 0)
	{
		if (ret == C_LOST)
			return (C_LOST);
		return (EXIT_FAILLURE);
	}
	ft_array_free(&split);
	return (ret);
}






int	switch_requet_client(int fd, char *requet)
{
	if (!ft_strncmp(requet, REQUET_QUIT, ft_strlen(REQUET_QUIT)))
		return (quit_requet_client(fd, requet));
	else if (!ft_strncmp(requet, REQUET_CD, ft_strlen(REQUET_CD)))
		return (cd_requet_client(fd, requet));
	else if (!ft_strncmp(requet, REQUET_PWD, ft_strlen(REQUET_PWD)))
		return (requet_client(fd, requet, STDOUT));
	else if (!ft_strncmp(requet, REQUET_GET, ft_strlen(REQUET_GET)))
		return (requet_get(fd, requet));
	else if (!ft_strncmp(requet, REQUET_PUT, ft_strlen(REQUET_GET)))
		return (requet_put(fd, requet));
	else if (!ft_strncmp(requet, REQUET_LS, ft_strlen(REQUET_LS)))
		return (requet_cmd(fd, requet, STDOUT));
	return (EXIT_SUCCESS);
}

/*
** TCP/IP (v4) client example from 42 school
*/

int	main(int ac, char **av)
{
	int					port;
	int					sock;

	if (ac != 3)
		usage(av[0]);
	if ((port = atoi(av[2])) <= 0)
		usage(av[0]);
	if ((sock = create_client(av[1], port)) < 0)
	 	return (EXIT_FAILLURE);

	char 	*line;
	char 	*line_trim;
	int		ret;

	ret = QUIT;
	line = NULL;
	line_trim = NULL;
	ft_putstr(PROMPT);
	while (get_next_line(STDIN, &line) > 0)
	{
		if (line)
			if ((line_trim = ft_strtrim(line)))
			{
				ret = switch_requet_client(sock, line_trim);
				free(line_trim);
				line_trim = NULL;
				if (ret == C_LOST || ret == QUIT)
					break ;
			}
		ft_putstr(PROMPT);
		ret = 0;
	}
	if (line)
		free(line);
	if (line_trim)
		free(line_trim);
	close(sock);
	if (ret == C_LOST)
	{
		ft_putstr_fd(ERROR, STDERR);
		ft_putstr_fd(CONNECTION_LOST, STDERR);
		return (EXIT_FAILLURE);
	}
	if (ret != QUIT)
	{
		ft_putstr_fd(ERROR, STDERR);
		ft_putstr_fd(INTERN_ERROR, STDERR);
		return (EXIT_FAILLURE);
	}
	return (EXIT_SUCCESS);
}
