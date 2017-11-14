/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_requet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/14 03:01:59 by alex              #+#    #+#             */
/*   Updated: 2017/11/14 04:49:22 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_p.h"

int	send_requet(int fd, uint32_t requet, uint32_t size, const void *data)
{
	void		*new;
	uint32_t	size_segment;

	size_segment = SIZE_HEADER + size;
	if (!(new = ft_strnew(size_segment)))
		return (EXIT_FAILLURE);
	ft_memcpy(new, &requet, sizeof(uint32_t));
	ft_memcpy((new + sizeof(uint32_t)), &size, sizeof(uint32_t));
	if (size && data)
	{
		ft_memcpy((new + SIZE_HEADER), data, size);
	}
	if (send(fd, new, size_segment, 0) == C_LOST)
	{
		free(new);
		return (EXIT_FAILLURE);
	}
	free(new);
	return (EXIT_SUCCESS);
}
