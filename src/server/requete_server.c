/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requete_server.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/26 00:32:56 by alex              #+#    #+#             */
/*   Updated: 2017/11/26 01:00:37 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

int	quit_requet(t_cs *cs)
{
	send_requet(
		cs->fd, R_QUIT, 0, NULL);
	return (QUIT);
}

int	pwd_requet(t_cs *cs, char **requet)
{
	size_t		size;
	char		*data;
	int			ref;

	if (ft_array_len((const void **)requet) > 1)
		return (send_error(cs->fd, TOO_MUCH_ARG));
	if (!ft_strcmp(cs->home, cs->pwd))
	{
		size = 1;
		ref = 0;
	}
	else
	{
		ref = ft_strlen(cs->home);
		size = ft_strlen(cs->pwd) - ref;
	}
	return (send_data_by_size(cs->fd, (cs->pwd + ref), size));
}

int	put_requet(t_cs *cs, char **requet, char *requet_s)
{
	int			size;
	char		*buf;
	int 		ret;

	if (ft_array_len((const void **)requet) != 2)
		return (send_error(cs->fd, INVALID_NB_ARG));
	if (send_requet(
		cs->fd, R_PUT_OK, 0, NULL) == C_LOST)
	{
			return (C_LOST);
	}
	if (get_reponse(cs->fd, requet_s))
	{
		return (send_error(cs->fd, TRANSFERT_FAIL));
	}
	return (send_success(cs->fd));
}

int	get_requet_server(t_cs *cs, char **requet)
{
	char *path_file;

	if (ft_array_len((const void **)requet) != 2)
		return (send_error(cs->fd, INVALID_NB_ARG));
	if (verify_dest(cs, requet[1]) == EXIT_FAILLURE)
		return (send_error(cs->fd, NO_ACCESS));
	if (*requet[1] == '/')
	{
		if (!(path_file = ft_strjoin(cs->home, requet[1])))
			return (send_error(cs->fd, INTERN_ERROR));
		free(requet[1]);
		requet[1] = path_file;
	}
	return (get_requet(cs->fd, requet, NO_LOG));
}
