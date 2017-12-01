/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/29 17:40:58 by aollivie          #+#    #+#             */
/*   Updated: 2017/12/01 13:01:08 by aollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

#define EXIT_FAILLURE	1
#define EXIT_SUCCESS	0

/*
** usage
*/

static void	usage(char *s)
{
	printf("Usage: %s <addresse> <port>\n", s);
	exit(EXIT_FAILLURE);
}

static void	main_loop_init(int *ret, char **line, char **line_trim)
{
	*ret = QUIT;
	*line = NULL;
	*line_trim = NULL;
}

static int	main_loop(int sock)
{
	char				*line;
	char				*line_trim;
	int					ret;

	main_loop_init(&ret, &line, &line_trim);
	ft_putstr(PROMPT);
	ret = 0;
	while (get_next_line(STDIN, &line) > 0)
	{
		if (line)
			if ((line_trim = ft_strtrim(line)))
			{
				ret = switch_requet_client(sock, line_trim);
				ft_str_free(&line_trim);
				if (ret == C_LOST || ret == QUIT)
					break ;
			}
		ft_str_free(&line);
		ft_putstr(PROMPT);
		ret = 0;
	}
	ft_str_free(&line);
	ft_str_free(&line_trim);
	return (ret);
}

/*
** TCP/IP (v4) client example from 42 school
*/

int			main(int ac, char **av)
{
	int					port;
	int					sock;
	int					ret;

	if (ac != 3)
		usage(av[0]);
	if ((port = atoi(av[2])) <= 0)
		usage(av[0]);
	if ((sock = create_client(av[1], av[2])) < 0)
		return (EXIT_FAILLURE);
	ret = main_loop(sock);
	close(sock);
	if (ret == C_LOST)
	{
		ft_putstr_fd(ERROR, STDERR);
		ft_putstr_fd(CONNECTION_LOST, STDERR);
	}
	if (!ret)
		printf("\n%s", SUCCESS);
	return (ret == QUIT ? EXIT_SUCCESS : EXIT_FAILLURE);
}
