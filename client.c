
#include "ft_p.h"

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
