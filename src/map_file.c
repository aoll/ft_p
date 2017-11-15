/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_file.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/15 05:29:24 by alex              #+#    #+#             */
/*   Updated: 2017/11/15 20:08:26 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_p.h"

int	map_file(const char *file_name, char **dst)
{
	int			fd;
	char		*ptr;
	struct stat	buf;

	if ((fd = open(file_name, O_RDONLY)) < 0)
	{
		return (-1);
	}
	if (fstat(fd, &buf) < 0)
		return (-1);
	if ((ptr = mmap(
		0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (-1);
	*dst = ptr;
	return (buf.st_size);
}
