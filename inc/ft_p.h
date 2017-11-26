/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_p.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aollivie <aollivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 16:42:38 by aollivie          #+#    #+#             */
/*   Updated: 2017/11/26 00:23:58 by alex             ###   ########.fr       */
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

# define IS_LOG						1
# define NO_LOG						0
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
# define INVALID_NB_ARG				"Command need one and only one argument\n"
# define UNKNOW_CMD					"Command not implemented\n"

# define TRANSFERT_FAIL				"Transfert fail\n"
# define CONNECTION_LOST			"Connection lost\n"
# define INTERN_ERROR				"Intern error\n"
# define NO_ACCESS					"No access\n"
# define SUCCESS					"SUCCESS\n"
# define ERROR						"ERROR\n"
# define QUIT						2
# define C_LOST						-1

# define EOT						-42

# define R_SUCCESS			200

# define R_CMD				100
# define R_WAIT_RECV		201
# define R_WAIT_SEND		202
# define R_RECV				203
# define R_GET_OK			204
# define R_PUT_OK			205
# define R_CMD_OK			206
# define R_ERROR			500
# define R_QUIT				420

# define REQUET_QUIT				"quit"
# define REQUET_CD					"cd"
# define REQUET_PWD					"pwd"
# define REQUET_GET					"get"
# define REQUET_PUT					"put"
# define REQUET_LS					"ls"

typedef struct	s_header
{
	uint32_t	requet;
	uint32_t	size;
}	t_header;

typedef struct	s_cs
{
	int			fd;
	char		*home;
	char		*pwd;
	char		*oldpwd;
}	t_cs;

int	send_success(int fd);
int	send_error(int fd, char *error);
int	send_requet(int fd, uint32_t requet, uint32_t size, const void *data);
int	wait_reponse(int fd, unsigned int reponse, int size, int is_log);
int	send_success(int fd);
int	send_error(int fd, char *error);
int	print_reponse(void *reponse);
int	map_file(const char *file_name, char **buf);
int	get_reponse(int fd, char *requet);
int	recv_by_size(int fd, int output);
int	get_requet(int fd, char **requet, int is_log);
int send_data_by_size(int fd, void *data, size_t size);

#endif
