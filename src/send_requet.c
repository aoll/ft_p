/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_requet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/14 03:01:59 by alex              #+#    #+#             */
/*   Updated: 2017/11/25 18:19:39 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_p.h"

int			print_reponse(void *reponse)
{
	t_header *header;

	header = (t_header *)reponse;
	if (header->requet == R_SUCCESS)
		ft_putstr(SUCCESS);
	else
		ft_putstr(ERROR);
	if (header->size > 0)
	{
		write(1, reponse + sizeof(header), header->size);
	}
	return (header->requet == R_SUCCESS ? EXIT_SUCCESS : EXIT_FAILLURE);
}

int			send_requet(
	int fd, uint32_t requet, uint32_t size, const void *data)
{
	void		*new;
	uint32_t	size_segment;

	new = NULL;
	size_segment = SIZE_HEADER;
	if (data)
		size_segment += size;
	if (!(new = ft_strnew(size_segment)))
		return (EXIT_FAILLURE);
	ft_memcpy(new, &requet, sizeof(uint32_t));
	ft_memcpy((new + sizeof(uint32_t)), &size, sizeof(uint32_t));
	if (size && data)
		ft_memcpy((new + SIZE_HEADER), data, size);
	if (send(fd, new, size_segment, 0) == C_LOST)
	{
		free(new);
		return (C_LOST);
	}
	free(new);
	return (EXIT_SUCCESS);
}

static int	recv_reponse(int fd, char **buf)
{
	int			ret;

	if (!(*buf = ft_strnew(RECV_SIZE)))
		return (-2);
	ft_bzero(*buf, RECV_SIZE);
	if ((ret = recv(fd, *buf, RECV_SIZE, 0)) == C_LOST)
		return (C_LOST);
	if (ret < (int)SIZE_HEADER)
	{
		free(*buf);
		return (C_LOST);
	}
	return (EXIT_SUCCESS);
}

int			wait_reponse(
	int fd, unsigned int reponse, int size, int is_log)
{
	char		*buf;
	t_header	*header;
	int			ret;
	if ((ret = recv_reponse(fd, &buf)))
		return (ret);
	header = (t_header *)buf;
	if (header->requet != reponse || ((int)header->size != size && size != -1))
	{
		if (is_log)
			print_reponse(buf);
		free(buf);
		return (-2);
	}
	if (is_log && reponse == R_SUCCESS)
		print_reponse(buf);
	size = header->size;
	free(buf);
	return (size);
}
