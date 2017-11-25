/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_reponse.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/25 18:18:47 by alex              #+#    #+#             */
/*   Updated: 2017/11/25 18:19:20 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_p.h"

int	send_error(int fd, char *error)
{
	if (send_requet(
		fd, R_ERROR, ft_strlen(error), error) == C_LOST)
		return (C_LOST);
	return (EXIT_FAILLURE);
}

int	send_success(int fd)
{
	if ((send_requet(fd, R_SUCCESS, 0
		, NULL)) == C_LOST)
		return (C_LOST);
	return (EXIT_SUCCESS);;
}
