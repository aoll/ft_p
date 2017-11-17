/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/17 13:34:01 by aollivie          #+#    #+#             */
/*   Updated: 2017/11/17 13:41:46 by aollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

#include "ft_p.h"

int			fork_process_cmd(int fd, char **arg);
int			cd_requet(t_cs *cs, char **requet);

#endif
