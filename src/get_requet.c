/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_requet.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/15 18:29:29 by alex              #+#    #+#             */
/*   Updated: 2017/11/25 22:51:56 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_p.h"

static int	send_data(int fd, void *src, size_t size)
{
	int		len;
	void	*ptr_end;
	int		ret;
	int		total;

	ptr_end = (void *)src + size;
	len = 0;
	total = 0;
	while (src < ptr_end)
	{
		len = RECV_SIZE;
		if ((ptr_end - src) < RECV_SIZE)
			len = ptr_end - src;
		if ((ret = send(fd, (const void*)src, len, 0) == C_LOST))
			return (C_LOST);
		src = (void *)src + len;
		total += len;
	}
	return (EXIT_SUCCESS);
}

int 		send_data_by_size(int fd, void *data, size_t size)
{
	if (send_requet(
		fd, R_WAIT_SEND, size, NULL) == C_LOST)
		return (C_LOST);
	if (wait_reponse(fd, R_WAIT_RECV, -1, NO_LOG) < 0)
		return (EXIT_FAILLURE);
	if (send_data(fd, data, size) == C_LOST)
		return (C_LOST);
	if (wait_reponse(fd, R_RECV, size, NO_LOG) < 0)
		return (send_error(fd, TRANSFERT_FAIL));
	return (send_success(fd));
}

static int	init_wait_get(fd, is_log)
{
	if (send_requet(
		fd, R_GET_OK, 0, NULL) == C_LOST)
	{
		return (C_LOST);
	}
	if (wait_reponse(fd, R_GET_OK, -1, is_log) < 0)
	{
		return (EXIT_FAILLURE);
	}
	return (EXIT_SUCCESS);
}

int			get_requet(int fd, char **requet, int is_log)
{
	int			size;
	char		*buf;
	int 		ret;

	if (ft_array_len((const void **)requet) != 2)
		return (send_error(fd, INVALID_NB_ARG));
	if ((size = map_file(requet[1], &buf)) < 0)
		return (send_error(fd, NO_ACCESS));
	if ((ret = init_wait_get(fd, is_log)) != EXIT_SUCCESS)
	{
		munmap(buf, size);
		return (send_error(fd, NO_ACCESS));
	}
	ret = send_data_by_size(fd, buf, size);
	if (munmap(buf, size) < 0)
		return (EXIT_FAILLURE);
	return (ret);
}
