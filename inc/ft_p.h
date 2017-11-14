/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_p.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 16:42:38 by aollivie          #+#    #+#             */
/*   Updated: 2017/11/14 08:55:00 by alex             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_P_H
# define FT_P_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
# include <sys/mman.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdlib.h>
# include "libft.h"
# include "client.h"

# define MAGIC_NUMER				42
# define SIZE_HEADER				(sizeof(uint32_t) * 2)

# define EXIT_FAILLURE				1
# define EXIT_SUCCESS				0

# define PROTOCOLE					"tcp"
# define NB_CONN_SOCKET				42

# define ERROR_TRANSFERT			"ERROR: transfert interupt\n"
# define MESS_LIMIT_NB_CON_REACHED	"TROP DE CONNECTIONS\n"
# define NEED_COMMAND_VALID			"Need a command with valid format"
# define PWD_MAX_LEN				256
# define RECV_SIZE					256

# define TOO_MUCH_ARG				"Too much argument for the command\n"
# define UNKNOW_CMD					"Command not implemented\n"

# define TRANSFERT_FAIL				"Transfert fail\n"
# define CONNECTION_LOST			"Connection lost\n"
# define INTERN_ERROR				"Intern error\n"
# define SUCCESS					"SUCCESS\n"
# define ERROR						"ERROR\n"
# define QUIT						2
# define C_LOST						-1

# define R_SUCCESS			200

# define R_CMD				100
# define R_WAIT_RECV		201
# define R_WAIT_SEND		202
# define R_RECV				203
# define R_ERROR			500
# define R_QUIT				420

# define REQUET_QUIT				"quit"
# define REQUET_CD					"cd"
# define REQUET_PWD					"pwd"

typedef struct	s_header
{
	uint32_t	requet;
	uint32_t	size;
}	t_header;

int	send_requet(int fd, uint32_t requet, uint32_t size, const void *data);

#endif
