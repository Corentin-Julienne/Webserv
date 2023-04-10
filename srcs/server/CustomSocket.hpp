/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:58 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/10 19:17:37 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CUSTOMSOCKET_HPP__
# define __CUSTOMSOCKET_HPP__

# include <sys/socket.h>	// for sockets in general
# include <sys/event.h>	// for kevents macros
# include <sys/stat.h>   // for isDirectory
# include <netinet/in.h>	// for the struct sockaddr_in
# include <cstring>		// for memset
# include <arpa/inet.h>	// for htonl and similar
# include <fcntl.h>		// for fcntl
# include <vector>
# include <sstream>      // for to_string equivalent
# include <fstream>
# include <map>
# include <iostream>
# include <unistd.h>
# include <cstdlib>
# include <dirent.h>
# include "../parser/ServConf.hpp"
# include "../parser/Location.hpp"
# include "../cgi/cgiLauncher.hpp"
# include "SocketInfos.hpp"
# include "include.hpp"

# define MAX_READ 16384

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

		void		read(int fd);
		void		write(int fd);

		int			getSocketFd();
		int			getPort();

//		std::string	getOutput(int fd);
//		void		setOutput(int fd, std::string output);
//		void 		removeOutput(int fd);

	private:
	
		std::string _getAbsoluteURIPath(const std::string uri);
		Location *	_getPathLocation(const std::string uri);
		void		_tryToIndex(std::string &filePath);
		void		_bindSocket(void);
		void		_enableSocketListening(void);
		void		_parseRequest(std::string req, std::string &reqType, std::string &uri, std::map<std::string, std::string> &headers);
		std::string	_GET(SocketInfos &infos, Location *loc);
		std::string	_POST(SocketInfos &infos);
		std::string	_DELETE(SocketInfos &infos, Location *loc);
		
		std::string	_generateError(size_t code, Location *location);
		size_t 		_isContentLengthValid(std::string reqType, std::map<std::string, std::string> headers, long long int maxBodySize);
		size_t		_isMethodAllowed(const std::string reqType, std::vector<std::string> allowedMethods);
		std::string _generateAutoIndex(const std::string path, const std::string relativePath, Location *loc);
		std::string	_generateFileContent(const std::string realFilePath, Location *loc);

		std::string	_assembleURI(SocketInfos &infos);
		std::string _detectRedirections(Location *loc);

	private:
	
		int													_domain;
		int													_type;
		int													_protocol;
		int													_backlog;
		int													_socket_fd;
		int													_kq;
		struct sockaddr_in									_sockaddr;
		int													_new_socket_fd;
		std::map< int, std::pair<std::string, std::string> > _outputs;
		ServConf											_servconf;
};

#endif
