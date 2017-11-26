/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/17 13:34:01 by aollivie          #+#    #+#             */
/*   Updated: 2017/11/26 00:34:25 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

#include <sys/types.h>
#include <sys/wait.h>
#include "ft_p.h"

int			fork_process_cmd(int fd, char **arg);
int			cd_requet(t_cs *cs, char **requet);
int			verify_dest(t_cs *cs, char *dest);
int			verify_multi_dest(t_cs *cs, char **requet);
int			new_process(int fd);
int			free_cs(t_cs *cs);
int			init_cs(t_cs *cs, int fd);
int			cmd_requet(t_cs *cs, char **requet);
int			quit_requet(t_cs *cs);
int			pwd_requet(t_cs *cs, char **requet);
int			put_requet(t_cs *cs, char **requet, char *requet_s);
int			get_requet_server(t_cs *cs, char **requet);

#endif
