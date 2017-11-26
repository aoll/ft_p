/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cs.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/26 00:29:10 by alex              #+#    #+#             */
/*   Updated: 2017/11/26 00:29:19 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

int	free_cs(t_cs *cs)
{
	if (cs->home)
		free(cs->home);
	if (cs->pwd)
		free(cs->pwd);
	if (cs->oldpwd)
		free(cs->oldpwd);
	return (EXIT_FAILLURE);
}

int	init_cs(t_cs *cs, int fd)
{
	if (!(cs->pwd = ft_strnew(PWD_MAX_LEN)))
		return (EXIT_FAILLURE);
	if (!(cs->pwd = getcwd(cs->pwd, PWD_MAX_LEN)))
		return (EXIT_FAILLURE);
	cs->fd = fd;
	cs->home = NULL;
	cs->oldpwd = NULL;
	if (!(cs->home = ft_strdup(cs->pwd)))
		return (free_cs(cs));
	if (!(cs->oldpwd = ft_strdup(cs->pwd)))
		return (free_cs(cs));
	return (EXIT_SUCCESS);
}
