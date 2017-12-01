/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   switch_requet_set_space.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/01 12:31:49 by aollivie          #+#    #+#             */
/*   Updated: 2017/12/01 13:01:41 by aollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_p.h"

int	switch_requet_set_space(char *requet)
{
	int	i;

	i = 0;
	while (requet[i] && i < RECV_SIZE)
	{
		if (requet[i] == '\t' || requet[i] == '\n')
		{
			requet[i] = ' ';
		}
		i++;
	}
	return (EXIT_SUCCESS);
}
