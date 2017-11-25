/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_requete.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/17 13:25:41 by aollivie          #+#    #+#             */
/*   Updated: 2017/11/24 15:07:41 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

int quit_requet_client(int fd, char *requet)
{
	send_requet(fd, R_CMD, ft_strlen(requet), (const void *)requet);
	ft_putstr(SUCCESS);
	return (QUIT);
}

int cd_requet_client(int fd, char *requet)
{
	char		*buf;
	int			ret;

	if ((ret = send_requet(fd, R_CMD, ft_strlen(requet),
	(const void *)requet)))
		return (ret);
	if (!(buf = ft_strnew(RECV_SIZE)))
		return (EXIT_FAILLURE);
	ret = recv(fd, buf, RECV_SIZE, 0);
	if (ret >= (int)SIZE_HEADER)
		print_reponse(buf);
	free(buf);
	return (ret <= 0 ? C_LOST : EXIT_SUCCESS);
}

int	requet_client(int fd, char *requet, int output)
{
	if (send_requet(fd, R_CMD, ft_strlen(requet),
	(const void *)requet))
		return (C_LOST);
	return (recv_by_size(fd, output));
}

int	read_result_cmd(int fd, int output)
{
	char	*buf;
	int		ret;

	ret = 0;
	if (!(buf = ft_strnew(RECV_SIZE)))
		return (EXIT_FAILLURE);
	ft_bzero(buf, RECV_SIZE);
	while ((ret = recv(fd, buf, RECV_SIZE, 0)) > 0)
	{
		if (buf[ret - 1] == EOT)
		{
			write(output, buf, ret - 1);
			break;
		}
		write(output, buf, ret);
		ft_bzero(buf, RECV_SIZE);
	}
	free(buf);
	if (ret == C_LOST)
		return (C_LOST);
	send_success(fd);
	return (wait_reponse(fd, R_SUCCESS, -1, IS_LOG));
}
