
#include "client.h"

#define EXIT_FAILLURE	1
#define EXIT_SUCCESS	0

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
	inet_pton(AF_INET, addr, &sin);
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(addr);
	sin.sin_family = AF_INET;
	if (connect(sock, (const struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		printf("%s\n", "Connect error");
		exit(EXIT_FAILLURE);
	}
	return (sock);
}

int	create_client6(char *addr, char *port)
{
	int					sock;
	struct protoent		*proto;
	struct in6_addr serveraddr;
	struct addrinfo hints;
	struct addrinfo *res=NULL;
	int rc;

	if (!(proto = getprotobyname(PROTOCOLE)))
		return (-1);
	hints.ai_flags    = AI_NUMERICSERV;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	rc = inet_pton(AF_INET, addr, &serveraddr);
	if (rc == 1)
	{
		hints.ai_family = AF_INET;
		hints.ai_flags |= AI_NUMERICHOST;
	}
	else
	{
		rc = inet_pton(AF_INET6, addr, &serveraddr);
		if (rc == 1)
		{
			hints.ai_family = AF_INET6;
			hints.ai_flags |= AI_NUMERICHOST;
		}
	}
	rc = getaddrinfo(addr, port, &hints, &res);
	if (rc != 0)
	{
		printf("Host not found --> %s\n", addr);
		return (-1);
	}
	sock = socket(res->ai_family, res->ai_socktype, proto->p_proto);
	if (sock < 0)
	{
		perror("socket() failed");
		return (-1);
	}
	while (res)
	{
		if (connect(sock, res->ai_addr, res->ai_addrlen) == EXIT_SUCCESS)
			return (sock);
		res = res->ai_next;
	}

	close(sock);
	perror("connect() failed");
	return (-1);
}

/*
** TCP/IP (v4) client example from 42 school
*/

int	main(int ac, char **av)
{
	int					port;
	int					sock;

	// return (get_addr(av));
	if (ac != 3)
		usage(av[0]);
	if ((port = atoi(av[2])) <= 0)
		usage(av[0]);
	if ((sock = create_client6(av[1], av[2])) < 0)
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
