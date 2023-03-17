/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:56 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/17 11:46:26 by spider-ma        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CustomSocket.hpp"

bool isDirectory(const std::string &path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false;
	return S_ISDIR(statbuf.st_mode);
}

CustomSocket::CustomSocket(ServConf server_config, int kq) : _domain(AF_INET), _type(SOCK_STREAM), _protocol(0), _backlog(10), _kq(kq), _new_socket_fd(-1), _servconf(server_config)
{
	_socket_fd = socket(_domain, _type, _protocol);
	if (_socket_fd < 0) {} // add function to handle errors
	_bindSocket();
	_enableSocketListening();
	std::cout << "Socket created on port " << _servconf._port << " (http://localhost:" << _servconf._port << "/)" << std::endl;
}

CustomSocket::~CustomSocket() 
{
	std::cout << "Closing socket" << std::endl;
	closeSocket(_socket_fd);
}

std::string CustomSocket::_getAbsoluteURIPath(const std::string uri)
{
	Location		*location = _getPathLocation(uri);
	std::string		absolutePath = (location ? location->_root : _servconf._root);

	return (absolutePath += uri);
}

Location* CustomSocket::_getPathLocation(const std::string uri)
{
	Location		*location = NULL;
	std::string		uriPath = uri;
	
	for (size_t i = 0; i < _servconf._locs.size(); i++)
	{
		if (uriPath.find(_servconf._locs[i]._url) == 0 && _servconf._locs[i]._url.size() > (location ? location->_url.size() : 0))
		{
			location = &(_servconf._locs[i]);
		}
	}
	return (location);
}

void CustomSocket::_tryToIndex(std::string &filePath)
{
	Location 					*location = _getPathLocation(filePath);
	std::ifstream				fileStream;
	std::vector<std::string> 	indexes;

	if (location)
		indexes = location->_index;
	else
		indexes = _servconf._index;

	std::cout << "Indexes: " << std::endl;
	for (size_t i = 0; i < indexes.size(); i++)
		std::cout << indexes[i] << std::endl;

	// First, check if the current filePath exists.
	// If it's not a valid check, it may be a valid folder path, so add a / at the end to know it.
	if (!isDirectory(filePath.c_str()))
	{
		return ;
	}
	else if (filePath.substr(filePath.size() - 1, 1) != "/")
	{
		filePath = filePath + "/";
	}

	// Since here we're sure that the filePath is a folder path, we can try to find index files in it.
	for (size_t i = 0; i < indexes.size(); i++)
	{
		if (access((filePath + indexes[i]).c_str(), F_OK) == 0)
		{
			filePath = filePath + indexes[i];
			break ;
		}
	}
}

std::string	CustomSocket::read(int fd)
{
	ssize_t	valret;

	char	buffer[1024]; // create a buffer to be used by read
	memset(buffer, 0, sizeof(buffer));
	valret = recv(fd, buffer, 1024, MSG_TRUNC/* | MSG_DONTWAIT*/); // manage case when len > 1024
	if (valret < 0)
	{
		std::cerr << "read operation: failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}
	std::cout << buffer << std::endl; // print buffer content in terminal, to get debug stuff

	std::string							buff = buffer;
	std::string							reqType, uri, body, output;
	std::map<std::string, std::string>	headers;
	this->_parseRequest(buff, reqType, uri, headers, body);
	if (reqType == "GET")
		output = this->_GET(uri);
	else if (reqType == "POST")
		output = this->_POST(uri, body);
	else if (reqType == "DELETE")
		output = this->_DELETE(uri, body);
	else
		output = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 9\n\nUNDEFINED";

	return (output);
}

void	CustomSocket::write(int fd, char const *output)
{
	ssize_t	valret;

	valret = send(fd, output, strlen(output), MSG_DONTWAIT);
	if (valret < 0)
	{
		std::cerr << "write operation: failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error here
	}
}

void	CustomSocket::_parseRequest(std::string req, std::string &reqType, std::string &uri, std::map<std::string, std::string> &headers, std::string &body)
{
	std::cout << req << std::endl;
	if (req.substr(0, 4) == "GET ")
		reqType = "GET";
	else if (req.substr(0, 5) == "POST ")
		reqType = "POST";
	else if (req.substr(0, 7) == "DELETE ")
		reqType = "DELETE";
	else
	{
		reqType = "UNDEFINED";
		return ;
	}
	size_t	i = req.find(" /") + 2;
	uri = req.substr(i, req.find(" ", i) - i);
	i = req.find("\n") + 1;
	while (i < req.length() && req[i] != '\n')
	{
		size_t		end_line_idx = req.find("\n", i);
		std::string	line = req.substr(i, end_line_idx - i);
		size_t		sep_idx = line.find(": ");
		if (sep_idx != line.npos)
		{
			std::string	key = line.substr(0, sep_idx);
			std::string	value = line.substr(sep_idx + 2);
			headers.insert(std::make_pair(key, value));
		}
		i = end_line_idx + 1;
	}
	if (++i < req.length())
		body = req.substr(i);
}

// private helper functions
void	CustomSocket::_bindSocket(void)
{
	memset((char *)&_sockaddr, 0, sizeof(_sockaddr)); // make sure struct is empty
	
	_sockaddr.sin_family = _domain;
	_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); // equal to 0.0.0.0
	_sockaddr.sin_port = htons(_servconf._port);
	
	if (bind(_socket_fd, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) < 0)
	{
		std::cerr << "bind operation : failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}
}

void	CustomSocket::_enableSocketListening(void)
{
	if (listen(_socket_fd, _backlog) < 0)
	{
		std::cerr << "listen operation : failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}
	// don't forget protections
	struct kevent	kev;
	EV_SET(&kev, this->_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, this);
	kevent(this->_kq, &kev, 1, NULL, 0, NULL);
}

void	CustomSocket::acceptConnection(void)
{
	socklen_t socketLen = sizeof(_sockaddr);
	if ((_new_socket_fd = accept(_socket_fd, (struct sockaddr *)&_sockaddr, &socketLen)) < 0)
	{
		std::cerr << "accept operation : failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error here
	}
	fcntl(this->_new_socket_fd, F_SETFL, O_NONBLOCK);
	struct kevent	events[2];
	EV_SET(&events[0], this->_new_socket_fd, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, this);
	EV_SET(&events[1], this->_new_socket_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE | EV_ONESHOT, 0, 0, this);
	kevent(this->_kq, events, 2, NULL, 0, NULL);
}

void	CustomSocket::closeSocket(int socket_fd)
{
	if (close(socket_fd) < 0)
	{
		std::cerr << "close operation : failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}
}

std::string	CustomSocket::_GET(std::string filePath)
{
	std::string			ret;
	std::ifstream		ifs;
	std::stringstream	content;
	
	std::string			realFilePath = _getAbsoluteURIPath(filePath);
	_tryToIndex(realFilePath);
	
	std::cout << "GET:" << std::endl << "\t- uri: " << filePath << std::endl << "\t- real path: " << realFilePath << std::endl;

	ifs.open(realFilePath.c_str());
	if (!ifs.is_open())
	{
		if (access(realFilePath.c_str(), F_OK) != 0)
			content << "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 9\n\nNot Found"; // TODO: Fit to HTTP norms
		else if (access(realFilePath.c_str(), R_OK) != 0)
			content << "HTTP/1.1 403 Forbidden\nContent-Type: text/plain\nContent-Length: 10\n\nForbidden"; // TODO: Fit to HTTP norms
	}
	else
	{
		std::stringstream	buff;
		buff << ifs.rdbuf();
		ifs.close();
		content << "HTTP/1.1 200 OK" << "\nContent-Type: text/html" << "\nContent-Length: " << buff.str().length() << "\n\n" << buff.str();
	}
	return (content.str());
}

std::string	CustomSocket::_POST(std::string filePath, std::string body)
{
	std::stringstream ss;
	std::string s = "POST\tat " + filePath + "\nbody:\n" + body;

	std::cout << body << std::endl;
	
	std::string			realFilePath = _getAbsoluteURIPath(filePath);
	_tryToIndex(realFilePath);

	ss << "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " << s.length() << "\n\n" << s;
	return (ss.str());
}

std::string	CustomSocket::_DELETE(std::string filePath, std::string body)
{
	std::stringstream 	ss;
	std::ifstream		ifs;
	std::string 		s = "DELETE\tat " + filePath + "\nbody:\n" + body;
	
	std::string			realFilePath = _getAbsoluteURIPath(filePath);
	_tryToIndex(realFilePath);

	ifs.open(realFilePath.c_str());
	if (ifs.is_open())
	{
		// delete the file
		ifs.close();
		std::remove(realFilePath.c_str());
	}
	else
	{
		// file does not exist
	}

	ss << "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " << s.length() << "\n\n" << s;
	return (ss.str());
}

int	CustomSocket::getSocketFd()
{
	return (this->_socket_fd);
}

int	CustomSocket::getPort()
{
	return (this->_servconf._port);
}

char	*CustomSocket::getOutput()
{
	return (this->_output);
}

void	CustomSocket::setOutput(char *output)
{
	this->_output = output;
}
