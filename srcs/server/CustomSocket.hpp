/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:58 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/07 13:19:47 by spider-ma        ###   ########.fr       */
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
#include <dirent.h>
#include "../parser/ServConf.hpp"
#include "../parser/Location.hpp"
#include "../cgi/cgiLauncher.hpp"
#include "SocketInfos.hpp"

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
		std::string	_extractQueryString(SocketInfos &infos);

		std::string	read(int fd);
		void		write(int fd, std::string output);

		int			getSocketFd();
		int			getPort();

		std::string	getOutput(int fd);
		void		setOutput(int fd, std::string output);
		void 		clearOutput(int fd);

	private:
	
		std::string _getAbsoluteURIPath(const std::string uri);
		Location *	_getPathLocation(const std::string uri);
		void		_tryToIndex(std::string &filePath);
		void		_bindSocket(void);
		void		_enableSocketListening(void);
		void		_parseRequest(std::string req, std::string &reqType, std::string &uri, std::map<std::string, std::string> &headers);
		std::string	_GET(SocketInfos &infos, Location *loc);
		std::string	_POST(SocketInfos &infos, Location *loc);
		std::string	_DELETE(SocketInfos &infos, Location *loc);
		
		std::string	_generateError(size_t code, Location *location);
		size_t 		_isContentLengthValid(std::string reqType, std::map<std::string, std::string> headers, long long int maxBodySize);
		size_t		_isMethodAllowed(const std::string reqType, std::vector<std::string> allowedMethods);
		std::string _generateAutoIndex(const std::string path, const std::string relativePath, Location *loc);
		std::string	_generateFileContent(const std::string realFilePath, Location *loc);

		std::string	_assembleURI(SocketInfos &infos);

	private:
	
		int							_domain;
		int							_type;
		int							_protocol;
		int							_backlog;
		int							_socket_fd;
		int							_kq;
		struct sockaddr_in			_sockaddr;
		int							_new_socket_fd;
		std::map<int, std::string> 	_outputs;
		ServConf					_servconf;
};
