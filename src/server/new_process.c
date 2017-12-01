/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_process.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/26 00:26:25 by alex              #+#    #+#             */
/*   Updated: 2017/12/01 15:12:54 by aollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

int			new_process(int fd)
{
	pid_t	pid;
	int		ret;

	if ((pid = fork()) == -1)
		return (EXIT_FAILLURE);
	if (!pid)
	{
		ret = read_socket(fd);
		close(fd);
	}
	return (EXIT_SUCCESS);
}
