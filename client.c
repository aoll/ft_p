
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

int	read_by_size(int fd, int output, size_t size)
{
	char	*buf;
	int		ret;
	size_t	read;

	read = 0;
	if (!(buf = ft_strnew(RECV_SIZE)))
		return (-1);
	while (read < size)
	{
		ft_bzero(buf, RECV_SIZE);
		if ((ret = recv(fd, buf, RECV_SIZE, 0)) == C_LOST)
		{
			free(buf);
			return (C_LOST);
		}
		write(output, buf, ret);
		if (!ret)
			break;
		read += ret;
	}
	free(buf);
	if (output == STDOUT)
		write(STDOUT, "\n", 1);
	return (read);
}

int	recv_by_size(int fd, int output)
{
	int			size;
	int			read;

	if ((size = wait_reponse(fd, R_WAIT_SEND, -1, IS_LOG)) < 0)
	{
		if (size == C_LOST)
			return (C_LOST);
		return (EXIT_FAILLURE);
	}
	if ((send_requet(fd, R_WAIT_RECV, 0, NULL)) == C_LOST)
		return (C_LOST);
	if ((read = read_by_size(fd, output, size)) == C_LOST)
		return (C_LOST);
	if ((send_requet(fd, R_RECV, read, NULL)) == C_LOST)
		return (C_LOST);
	if ((size = wait_reponse(fd, R_SUCCESS, -1, IS_LOG)) < 0)
	{
		if (size == C_LOST)
			return (C_LOST);
		return (EXIT_FAILLURE);
	}
	return (EXIT_SUCCESS);
}

int	requet_client(int fd, char *requet, int output)
{
	if (send_requet(fd, R_CMD, ft_strlen(requet),
	(const void *)requet))
		return (C_LOST);
	return (recv_by_size(fd, output));
}



int	create_file(char *requet)
{
	char	**split;
	int		fd;

	if (!(split = ft_strsplit(requet, ' ')))
		return (-1);
	if (!split[1])
	{
		ft_array_free(&split);
		return (-1);
	}
	if ((fd = open(split[1], O_CREAT | O_RDWR | O_TRUNC,
	   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)) < 0)
	{
		ft_array_free(&split);
		return (-1);
	}
	ft_array_free(&split);
	return (fd);
}

int	requet_get(int fd, char *requet)
{
	int	ret;
	int dest_fd;

	if (send_requet(fd, R_CMD, ft_strlen(requet),
		(const void *)requet) == C_LOST)
		return (C_LOST);
	if ((ret = wait_reponse(fd, R_GET_OK, -1, IS_LOG)) < 0)
	{
		if (ret == C_LOST)
			return (C_LOST);
		return (EXIT_FAILLURE);
	}
	if ((dest_fd = create_file(requet)) < 0)
		return (send_error(fd, NO_ACCESS));
	if ((send_requet(fd, R_GET_OK, 0, NULL)) == C_LOST)
		return (C_LOST);
	ret = recv_by_size(fd, dest_fd);
	close(dest_fd);
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
	return (EXIT_SUCCESS);
}

int	test(char *s)
{
	char	*buf;
	int		size;

	if ((size = map_file(s, &buf)) < 0)
		return (EXIT_FAILLURE); //no access
	write(1, buf, size);
	if (munmap(buf, size) < 0)
		return (-1);
	return (1);
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
