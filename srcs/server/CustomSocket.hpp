/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:58 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/01 13:56:26 by spider-ma        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>	// for sockets in general
#include <netinet/in.h>	// for the struct sockaddr_in
#include <cstring>		// for memset
#include <arpa/inet.h>	// for htonl and similar
#include <unistd.h>		// for close
#include <poll.h>		// for poll
#include <iostream>
#include <stdlib.h>

// used to create a socket and setup it to be used later
class CustomSocket
{
	public:
	
		CustomSocket(void);
		~CustomSocket();

		void	startServer(void);

	private:

		void	_bindSocket(void);
		void	_enableSocketListening(void);
		void	_acceptConnection(void);
		void	_closeSocket(int socket_fd);
		std::string	_GET(std::string filePath);
		std::string	_POST(std::string filePath, std::string body);
		std::string	_DELETE(std::string filePath, std::string body);

	private:
	
		int					_domain;
		int					_type;
		int					_protocol;
		const int			_port;
		int					_backlog;
		int					_socket_fd;
		struct sockaddr_in	_sockaddr;
		int					_new_socket_fd;
};
