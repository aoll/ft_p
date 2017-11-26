/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verfi_dest.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/16 00:06:02 by alex              #+#    #+#             */
/*   Updated: 2017/11/26 00:22:49 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_p.h"

int	verify_dest(t_cs *cs, char *dest)
{
	char	**split;
	int		i;
	int		dir;

	if (!(split = ft_strsplit(dest, '/')))
		return (EXIT_FAILLURE);
	i = 0;
	dir = ft_nb_c(cs->pwd + ft_strlen(cs->home), '/');
	while (split[i])
	{
		if (dir < 0)
			break ;
		if (!ft_strcmp("..", split[i]))
			dir--;
		else
			dir++;
		i++;
	}
	ft_array_free(&split);
	if (dir < 0)
		return (EXIT_FAILLURE);
	return (EXIT_SUCCESS);
}

int	verify_multi_dest(t_cs *cs, char **requet)
{
	int i;

	i = 0;
	if (!*requet)
		return (EXIT_SUCCESS);
	while (requet[i] && *requet[i] == '-')
		i++;
	while (requet[i])
	{
		if (verify_dest(cs, requet[i]))
			return (EXIT_FAILLURE);
		i++;
	}
	return (EXIT_SUCCESS);
}
