/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_process.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/26 00:26:25 by alex              #+#    #+#             */
/*   Updated: 2017/12/03 16:51:49 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

int			new_process(int fd)
{
	pid_t	pid;
	int		ret;
	int		status;

	if ((pid = fork()) == -1)
		return (EXIT_FAILLURE);
	if (!pid)
	{
		ret = read_socket(fd);
		close(fd);
	}
	else
	{
		wait4(pid, &status, WNOHANG, NULL);
	}
	return (EXIT_SUCCESS);
}
