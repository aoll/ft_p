/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/17 13:31:40 by aollivie          #+#    #+#             */
/*   Updated: 2017/11/24 16:31:03 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

static int	dup_std(int fd1_dst, int fd1_src, int fd2_dst, int fd2_src)
{
	if (dup2(fd1_dst, fd1_src) == -1 || dup2(fd2_dst, fd2_src) == -1)
		return (EXIT_FAILLURE);
	return (EXIT_SUCCESS);
}


int			fork_process_cmd(int fd, char **arg)
{
	pid_t 	pid;
	int		ret;
	int status;

	if ((pid = fork()) == -1)
		return (-1);
	if (!pid)
	{
		if (dup_std(fd, STDOUT, fd, STDERR))
			return (send_error(fd, INTERN_ERROR));
		execv("/bin/ls", arg);
		exit(-1);
	}
	else
	{
		wait(&status);
		ret = WEXITSTATUS(status);
		return (ret);
	}
	return (EXIT_SUCCESS);
}
