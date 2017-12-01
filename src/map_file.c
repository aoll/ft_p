/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_file.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/15 05:29:24 by alex              #+#    #+#             */
/*   Updated: 2017/12/01 13:00:07 by aollivie         ###   ########.fr       */
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
	close(fd);
	return (buf.st_size);
}
