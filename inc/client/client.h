/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alex <alex@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/14 05:07:02 by alex              #+#    #+#             */
/*   Updated: 2017/11/17 13:44:17 by aollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
# define CLIENT_H

# define PROMPT	"ft_p> "
# define IS_LOG	1

#include "ft_p.h"

int			switch_requet_client(int fd, char *requet);
int			requet_put(int fd, char *requet);
int			requet_get(int fd, char *requet);
int			requet_cmd(int fd, char *requet, int output);
int			quit_requet_client(int fd, char *requet);
int			cd_requet_client(int fd, char *requet);
int			requet_client(int fd, char *requet, int output);
int			read_result_cmd(int fd, int output);
#endif
