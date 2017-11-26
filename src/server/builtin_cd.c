/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/17 13:38:02 by aollivie          #+#    #+#             */
/*   Updated: 2017/11/26 00:25:06 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

static int	execute_cd(t_cs *cs, char *pwd)
{
	char *buf;

	printf("pwd: %s\n", pwd);
	if (chdir(pwd))
		return (send_error(cs->fd, NO_ACCESS));
	if (!(buf = getcwd(NULL, RECV_SIZE)))
		return (send_error(cs->fd, INTERN_ERROR));
	free(cs->oldpwd);
	cs->oldpwd = cs->pwd;
	cs->pwd = buf;
	send_requet(
		cs->fd, R_SUCCESS, 0, NULL);
	return (EXIT_SUCCESS);
}

static char	*new_pwd_target(t_cs *cs, char **requet)
{
	char	*target;

	if (*requet[1] != '/')
	{
		if (!(target = ft_strjoin(cs->pwd, "/")))
			return (NULL);
		if (!(target = ft_strjoin_free(&target, requet[1])))
			return (NULL);
	}
	else
	{
		if (!(target = ft_strjoin(cs->home, requet[1])))
			return (NULL);
	}
	return (target);
}

int			cd_requet(t_cs *cs, char **requet)
{
	int		ret;
	int		len;
	char	*target;

	if ((len = ft_array_len((const void **)requet)) < 2)
		return (execute_cd(cs, cs->home));
	else if (len > 2)
		return (send_error(cs->fd, TOO_MUCH_ARG));
	if (!ft_strcmp(requet[1], "-"))
		return (execute_cd(cs, cs->oldpwd));
	if (!(target = new_pwd_target(cs, requet)))
		return (send_error(cs->fd, INTERN_ERROR));
	if (verify_dest(cs, requet[1]) == EXIT_FAILLURE)
	{
		free(target);
		return (send_error(cs->fd, NO_ACCESS));
	}
	ret = execute_cd(cs, target);
	free(target);
	return (ret);
}
