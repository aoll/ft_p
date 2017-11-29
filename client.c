/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/29 17:40:58 by aollivie          #+#    #+#             */
/*   Updated: 2017/11/29 17:42:08 by aollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

int		create_client_ipv4(char *addr, int port)
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

/*
** TCP/IP (v4) client example from 42 school
*/

int		main(int ac, char **av)
{
	int					port;
	int					sock;
	char				*line;
	char				*line_trim;
	int					ret;

	if (ac != 3)
		usage(av[0]);
	if ((port = atoi(av[2])) <= 0)
		usage(av[0]);
	if ((sock = create_client(av[1], av[2])) < 0)
		return (EXIT_FAILLURE);
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
		free(line);
		line = NULL;
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
