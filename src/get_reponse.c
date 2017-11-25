/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_reponse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/15 17:56:44 by alex              #+#    #+#             */
/*   Updated: 2017/11/25 18:03:53 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_p.h"

static int	read_by_size(int fd, int output, size_t size)
{
	char	*buf;
	int		ret;
	size_t	read;

	read = 0;
	if (!(buf = ft_strnew(RECV_SIZE)))
		return (-1);
	while (read < size)
	{
		ft_bzero(buf, RECV_SIZE);
		if ((ret = recv(fd, buf, RECV_SIZE, 0)) == C_LOST)
		{
			free(buf);
			return (C_LOST);
		}
		write(output, buf, ret);
		if (!ret)
			break;
		read += ret;
	}
	free(buf);
	return (read);
}

int	recv_by_size(int fd, int output)
{
	int			size;
	int			read;

	if ((size = wait_reponse(fd, R_WAIT_SEND, -1, IS_LOG)) < 0)
		return (size == C_LOST ? C_LOST : EXIT_FAILLURE);
	if ((send_requet(fd, R_WAIT_RECV, 0, NULL)) == C_LOST)
		return (C_LOST);
	if ((read = read_by_size(fd, output, size)) == C_LOST)
		return (C_LOST);
	if (output == STDOUT)
		write(STDOUT, "\n", 1);
	if ((send_requet(fd, R_RECV, read, NULL)) == C_LOST)
		return (C_LOST);
	if ((size = wait_reponse(fd, R_SUCCESS, -1, IS_LOG)) < 0)
		return (size == C_LOST ? C_LOST : EXIT_FAILLURE);
	return (EXIT_SUCCESS);
}

static int	create_file(char *requet)
{
	char	**split;
	char	**dest;
	int		fd;
	int		len;

	if (!(split = ft_strsplit(requet, ' ')))
		return (-1);
	if (!split[1])
	{
		ft_array_free(&split);
		return (-1);
	}
	if (!(dest = ft_strsplit(*(split + 1), '/')))
		return (-1);
	len = ft_array_len((const void **)dest);
	fd = open(dest[len - 1], O_CREAT | O_RDWR | O_TRUNC,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	ft_array_free(&split);
	ft_array_free(&dest);
	return (fd < 0 ? -1 : fd);
}

int			get_reponse(int fd, char *requet)
{
	int	ret;
	int dest_fd;

	if ((ret = wait_reponse(fd, R_GET_OK, -1, IS_LOG)) < 0)
		return (ret == C_LOST ? C_LOST : EXIT_FAILLURE);
	if ((dest_fd = create_file(requet)) < 0)
		return (send_error(fd, NO_ACCESS));
	if ((send_requet(fd, R_GET_OK, 0, NULL)) == C_LOST)
		return (C_LOST);
	ret = recv_by_size(fd, dest_fd);
	close(dest_fd);
	return (ret);
}
