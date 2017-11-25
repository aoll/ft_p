/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_client.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/25 17:35:48 by alex              #+#    #+#             */
/*   Updated: 2017/11/25 22:21:40 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

static void			init_addrinfo(
	char * addr, struct addrinfo *hints, struct in6_addr *serveraddr)
{
	int rc;

	hints->ai_flags = AI_NUMERICSERV;
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = SOCK_STREAM;
	rc = inet_pton(AF_INET, addr, serveraddr);
	if (rc == 1)
	{
		hints->ai_family = AF_INET;
		hints->ai_flags |= AI_NUMERICHOST;
	}
	else
	{
		rc = inet_pton(AF_INET6, addr, serveraddr);
		if (rc == 1)
		{
			hints->ai_family = AF_INET6;
			hints->ai_flags |= AI_NUMERICHOST;
		}
	}
	return;
}

static void				free_res(struct addrinfo *res)
{
	if (!res)
		return ;
	free_res(res->ai_next);
	free(res);
}

static struct addrinfo	*get_addrinfo(char *addr, char *port)
{
	struct in6_addr		serveraddr;
	struct addrinfo		*hints;
	int					rc;
	struct addrinfo		*res;

	rc = -1;
	res = NULL;
	if (!(hints = malloc(sizeof(*hints))))
		return (NULL);
	ft_memset(hints, 0, sizeof(*hints));
	init_addrinfo(addr, hints, &serveraddr);
	rc = getaddrinfo(addr, port, hints, &res);
	free(hints);
	if (rc != 0)
	{
		printf("Host not found --> %s\n", addr);
		return (NULL);
	}
	return (res);
}

static int	con_socket(int fd, struct addrinfo *res)
{
	while (res)
	{
		if (connect(fd, res->ai_addr, res->ai_addrlen) == EXIT_SUCCESS)
		{
			return (EXIT_SUCCESS);
		}
		res = res->ai_next;
	}
	return (EXIT_FAILLURE);
}

int	create_client(char *addr, char *port)
{
	int					sock;
	struct protoent		*proto;
	struct addrinfo		*res;
	int					ret;

	res = NULL;
	if (!(proto = getprotobyname(PROTOCOLE)))
		return (-1);
	if (!(res = get_addrinfo(addr, port)))
		return (-1);
	sock = socket(res->ai_family, res->ai_socktype, proto->p_proto);
	if (sock < 0)
	{
		ft_putstr_fd("socket() failed\n", STDERR);
		return (-1);
	}
	ret = con_socket(sock, res);
	free_res(res);
	if (!ret)
		return (sock);
	close(sock);
	ft_putstr_fd("connect() failed\n", STDERR);
	return (-1);
}
