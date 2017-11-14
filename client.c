
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

int	print_reponse(void *reponse)
{
	t_header *header;

	header = (t_header *)reponse;
	if (header->requet == R_SUCCESS)
		ft_putstr(SUCCESS);
	else
		ft_putstr(ERROR);
	if (header->size)
	{
		ft_putstr(reponse + sizeof(header));
	}
	return (header->requet == R_SUCCESS ? EXIT_SUCCESS : EXIT_FAILLURE);
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

int	read_by_size(int fd, char *reponse, int output)
{
	t_header	*header;
	int			read;
	int			ret;
	char		*buf;

	read = 0;
	header = (t_header *)reponse;
	if (header->requet != R_WAIT_SEND)
	{
		print_reponse(reponse);
		return (EXIT_SUCCESS);
	}
	if (!(buf = ft_strnew(RECV_SIZE)))
		return (EXIT_FAILLURE);
	if ((ret = send_requet(fd, R_WAIT_RECV, 0, NULL)))
	{
		free(buf);
		return (ret);
	}
	while (read < (int)header->size)
	{
		ft_bzero(buf, RECV_SIZE);
		if ((ret = recv(fd, buf, RECV_SIZE, 0)) == C_LOST)
		{
			free(buf);
			return (ret);
		}
		write(output, buf, ret);
		if (!ret)
			break;
		read += ret;
	}
	if ((ret = send_requet(fd, R_RECV, read, NULL)) == C_LOST)
		return (ret);

	ft_bzero(buf, RECV_SIZE);
	if ((ret = recv(fd, buf, RECV_SIZE, 0)) == C_LOST)
	{
		free(buf);
		return (ret);
	}
	if (ret >= (int)SIZE_HEADER)
		print_reponse(buf);
	else
	{
		ft_putstr(ERROR_TRANSFERT);
		return (EXIT_FAILLURE);
	}
	return (EXIT_SUCCESS);
}

int	requet_client(int fd, char *requet, int output)
{
	int			ret;
	char		*buf;

	if ((ret = send_requet(fd, R_CMD, ft_strlen(requet),
	(const void *)requet)))
		return (ret);
	if (!(buf = ft_strnew(RECV_SIZE)))
		return (EXIT_FAILLURE);
	ret = recv(fd, buf, RECV_SIZE, 0);
	if (ret >= (int)SIZE_HEADER)
		ret = read_by_size(fd, buf, output);
	free(buf);
	return (ret);
	// return (ret == C_LOST ? C_LOST : EXIT_SUCCESS);
}

int	switch_requet_client(int fd, char *requet)
{

	if (!ft_strncmp(requet, REQUET_QUIT, ft_strlen(REQUET_QUIT)))
		return (quit_requet_client(fd, requet));
	else if (!ft_strncmp(requet, REQUET_CD, ft_strlen(REQUET_CD)))
		return (cd_requet_client(fd, requet));
	else if (!ft_strncmp(requet, REQUET_PWD, ft_strlen(REQUET_PWD)))
		return (requet_client(fd, requet, STDOUT));
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
				if (ret)
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
