/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_process.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/26 00:26:25 by alex              #+#    #+#             */
/*   Updated: 2017/12/01 17:58:48 by aollivie         ###   ########.fr       */
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
		wait4(-1, &status, 0, NULL);
		close(fd);
		printf("%s\n", "end deco");
	}
	return (EXIT_SUCCESS);
}
