/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:58 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/13 18:12:41 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>	// for sockets in general
#include <sys/event.h>	// for kevents macros
#include <sys/stat.h>   // for isDirectory
#include <netinet/in.h>	// for the struct sockaddr_in
#include <cstring>		// for memset
#include <arpa/inet.h>	// for htonl and similar
#include <unistd.h>		// for close
#include <fcntl.h>		// for fcntl
#include <vector>
#include <poll.h>		// for poll
#include <sstream>      // for to_string equivalent
#include <fstream>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "../parser/ServConf.hpp"
#include "../parser/Location.hpp"

bool	isDirectory(const std::string &path);

// used to create a socket and setup it to be used later
class CustomSocket
{
	public:
	
		CustomSocket(void);
		CustomSocket(ServConf server_conf, int kq);
		~CustomSocket();

		void		acceptConnection(void);
		void		closeSocket(int socket_fd);
		std::string	read(int fd);
		void		write(int fd, char const *output);

		int			getSocketFd();
		char		*getOutput();

		void		setOutput(char *output);

	private:

		void		_bindSocket(void);
		void		_enableSocketListening(void);
		std::string _getAbsoluteURIPath(const std::string uri);
		Location *	_getPathLocation(const std::string uri);
		void		_tryToIndex(std::string &filePath);
		void		_bindSocket(void);
		void		_enableSocketListening(void);
		void		_parseRequest(std::string req, std::string &reqType, std::string &uri, std::map<std::string, std::string> &headers, std::string &body);
		std::string	_GET(std::string filePath);
		std::string	_POST(std::string filePath, std::string body);
		std::string	_DELETE(std::string filePath, std::string body);

	private:
	
		int					_domain;
		int					_type;
		int					_protocol;
		int					_backlog;
		int					_socket_fd;
		int					_kq;
		struct sockaddr_in	_sockaddr;
		int					_new_socket_fd;
		char				*_output;
		ServConf			_servconf;
};
