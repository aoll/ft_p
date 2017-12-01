/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/29 17:39:04 by aollivie          #+#    #+#             */
/*   Updated: 2017/12/01 17:53:01 by aollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "server.h"

/*
** usage
*/

static void	usage(char *s)
{
	dup2(STDERR, STDOUT);
	printf("Usage: %s <port> <option dir>\n", s);
	exit(EXIT_FAILLURE);
}

static int	create_server(int port)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in6	sin;

	if (!(proto = getprotobyname(PROTOCOLE)))
		return (-1);
	ft_memset(&sin, 0, sizeof(sin));
	sock = socket(PF_INET6, SOCK_STREAM, proto->p_proto);
	sin.sin6_family = AF_INET6;
	sin.sin6_port = htons(port);
	sin.sin6_addr = in6addr_any;
	bind(sock, (const struct sockaddr *)&sin, sizeof(sin));
	listen(sock, NB_CONN_SOCKET);
	return (sock);
}

static int	main_loop(int sock)
{
	int					cs;
	unsigned int		cslen;
	struct sockaddr_in	csin;

	while (42)
	{
		ft_memset(&csin, 0, sizeof(csin));
		ft_memset(&cslen, 0, sizeof(cslen));
		if ((cs = accept(sock, (struct sockaddr *)&csin, &cslen)) == -1)
		{
			ft_putstr_fd(MESS_LIMIT_NB_CON_REACHED, STDERR);
			continue ;
		}
		new_process(cs);
	}
}

static void	set_start_dir(const char *path)
{
	if (chdir(path))
	{
		dup2(STDERR, STDOUT);
		printf("Error: no access to %s\n", path);
		exit(EXIT_FAILLURE);
	}
}

/*
** TCP/IP (v4) server example from 42 school
*/

int			main(int ac, char **av)
{
	int					port;
	int					sock;

	if (ac < 2 || ac > 3)
		usage(av[0]);
	if ((port = atoi(av[1])) <= 0)
		usage(av[0]);
	if (ac == 3)
		set_start_dir(av[2]);
	if ((sock = create_server(port)) < 0)
		return (EXIT_FAILLURE);
	main_loop(sock);
	close(sock);
	return (EXIT_SUCCESS);
}
