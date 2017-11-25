/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requete.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/17 13:17:42 by aollivie          #+#    #+#             */
/*   Updated: 2017/11/25 22:52:43 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.h"

int			requet_cmd(int fd, char *requet, int output)
{
	int ret;

	if (send_requet(fd, R_CMD, ft_strlen(requet),
	(const void *)requet))
		return (C_LOST);
	if ((ret = wait_reponse(fd, R_CMD_OK, -1, IS_LOG)))
		return (ret);
	if (send_requet(fd, R_WAIT_RECV, 0, NULL))
			return (C_LOST);
	return (read_result_cmd(fd, output));
}

int			requet_get(int fd, char *requet)
{
	if (send_requet(fd, R_CMD, ft_strlen(requet),
		(const void *)requet) == C_LOST)
		return (C_LOST);
	return (get_reponse(fd, requet));
}

static int check_file(char **split)
{
	int			fd;

	if (!*(split + 1) || *(split + 2))
	{
		printf("ERROR\n%s", INVALID_NB_ARG);
		return (EXIT_FAILLURE);
	}
	if ((fd = open(*(split + 1), O_RDONLY)) < 0)
	{
		printf("ERROR\n%s", NO_ACCESS);
		return (EXIT_FAILLURE);
	}
	close(fd);
	return (EXIT_SUCCESS);
}


int			requet_put(int fd, char *requet)
{
	int		ret;
	char	**split;

	if (!(split = ft_strsplit(requet, ' ')))
		return (EXIT_FAILLURE);
	if (check_file(split))
		return (EXIT_FAILLURE);
	if (send_requet(fd, R_CMD, ft_strlen(requet),
		(const void *)requet) == C_LOST)
		return (C_LOST);
	if ((ret = wait_reponse(fd, R_PUT_OK, -1, IS_LOG)) < 0)
		return (ret == C_LOST ? C_LOST : EXIT_FAILLURE);
	ret = get_requet(fd, split, IS_LOG);
	if ((ret = wait_reponse(fd, R_SUCCESS, -1, IS_LOG)) < 0)
		return (ret == C_LOST ? C_LOST : EXIT_FAILLURE);
	ft_array_free(&split);
	return (ret);
}

int			switch_requet_client(int fd, char *requet)
{
	if (!ft_strncmp(requet, REQUET_QUIT, ft_strlen(REQUET_QUIT)))
		return (quit_requet_client(fd, requet));
	else if (!ft_strncmp(requet, REQUET_CD, ft_strlen(REQUET_CD)))
		return (cd_requet_client(fd, requet));
	else if (!ft_strncmp(requet, REQUET_PWD, ft_strlen(REQUET_PWD)))
		return (requet_client(fd, requet, STDOUT));
	else if (!ft_strncmp(requet, REQUET_GET, ft_strlen(REQUET_GET)))
		return (requet_get(fd, requet));
	else if (!ft_strncmp(requet, REQUET_PUT, ft_strlen(REQUET_GET)))
		return (requet_put(fd, requet));
	else if (!ft_strncmp(requet, REQUET_LS, ft_strlen(REQUET_LS)))
		return (requet_cmd(fd, requet, STDOUT));
	return (EXIT_SUCCESS);
}
