/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:56 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/27 18:48:02 by mpeharpr         ###   ########.fr       */
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
	int	so_reuseaddr = 1;
	_socket_fd = socket(_domain, _type, _protocol);
	if (_socket_fd < 0) {} // add function to handle errors
	setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));
	fcntl(this->_socket_fd, F_SETFL, O_NONBLOCK);
	_bindSocket();
	_enableSocketListening();
	std::cout << "Socket created on port " << _servconf._port << " (http://localhost:" << _servconf._port << "/)" << std::endl;
}

CustomSocket::~CustomSocket() 
{
	std::cout << "Closing socket" << std::endl;
	closeSocket(_socket_fd);
}

void	CustomSocket::_parseRequest(std::string req, std::string &reqType, std::string &uri, std::map<std::string, std::string> &headers, std::string &body)
{
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
	memset((char *)&this->_sockaddr, 0, sizeof(this->_sockaddr)); // make sure struct is empty
	
	this->_sockaddr.sin_family = this->_domain;
	this->_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); // equal to 0.0.0.0
//	this->_sockaddr.sin_addr.s_addr = htonl(this->_servconf._ip_address);
	this->_sockaddr.sin_port = htons(this->_servconf._port);
	
	if (bind(this->_socket_fd, (struct sockaddr *)&this->_sockaddr, sizeof(this->_sockaddr)) < 0)
	{
		std::cerr << "bind: ";
		std::cout << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}
}

void	CustomSocket::_enableSocketListening(void)
{
	if (listen(_socket_fd, _backlog) < 0)
	{
		std::cerr << "listen: ";
		std::cout << strerror(errno) << std::endl;
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
		std::cerr << "accept: ";
		std::cout << strerror(errno) << std::endl;
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
		std::cerr << "close: ";
		std::cout << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}
}

int	CustomSocket::getSocketFd()
{
	return (this->_socket_fd);
}

int	CustomSocket::getPort()
{
	return (this->_servconf._port);
}

std::string	CustomSocket::getOutput()
{
	return (this->_output);
}

void	CustomSocket::setOutput(std::string output)
{
	this->_output = output;
}

std::string	CustomSocket::read(int fd)
{
	ssize_t	valret;

	char	buffer[1024 * 10]; // create a buffer to be used by read
	memset(buffer, 0, sizeof(buffer));
	valret = recv(fd, buffer, 1024 * 10, MSG_TRUNC/* | MSG_DONTWAIT*/); // manage case when len > 1024
	if (valret < 0)
	{
		std::cerr << "recv: ";
		std::cout << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}

	std::string							buff = buffer;
	std::string							reqType, uri, body, output;
	std::map<std::string, std::string>	headers;
	this->_parseRequest(buff, reqType, uri, headers, body);
	
	// Add the suffix to the uri if it's a directory
	if (uri.substr(0, 1) != "/")
		uri = "/" + uri;

	Location 	*loc = _getPathLocation(uri);
	size_t		code = _isMethodAllowed(reqType, (loc ? loc->_allowed_http_methods : _servconf._allowed_http_methods));

	if (code == 200)
		code = _isContentLengthValid(reqType, headers, (loc ? loc->_client_max_body_size : _servconf._client_max_body_size));

	if (code == 200)
	{
		if (reqType == "GET")
			output = _GET(uri, loc);
		else if (reqType == "POST")
			output = _POST(uri, body, loc);
		else if (reqType == "DELETE")
			output = _DELETE(uri, body, loc);
		else
			output = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 9\n\nUNDEFINED";
	}
	else
		_generateError(code, output);
	
	std::cout << "-> Code for above request is " << code << std::endl;
	return (output);
}

void	CustomSocket::write(int fd, std::string output)
{
	ssize_t	valret;

	valret = send(fd, output.c_str(), output.length(), MSG_DONTWAIT);
	if (valret < 0)
	{
		std::cerr << "send: ";
		std::cout << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
		// handle error here
	}
}

std::string	CustomSocket::_GET(std::string filePath, Location *loc)
{
	std::string			ret;
	std::stringstream	content;
	std::string			realFilePath = _getAbsoluteURIPath(filePath);

	_tryToIndex(realFilePath);
	std::cout << "GET:" << std::endl << "\t- uri: " << filePath << std::endl << "\t- real path: " << realFilePath << std::endl;
	
	bool isDirectory = (realFilePath.substr(realFilePath.length() - 1, 1) == "/");
	if (isDirectory)
	{
		if (filePath.substr(filePath.length() - 1, 1) != "/")
			filePath += "/";
		if ((loc ? loc->_autoindex : _servconf._autoindex))
			content << _generateAutoIndex(realFilePath, filePath);
		else
			content << "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 0\n\n"; // TODO: Fit to HTTP norms
	}
	else
	{
		content << _generateFileContent(realFilePath);
	}
	
	return (content.str());
}

std::string	CustomSocket::_POST(std::string filePath, std::string body, Location *loc)
{
	std::stringstream ss;
	std::string s = "POST\tat " + filePath + "\nbody:\n" + body;

	loc = (Location*)loc; // REMOVE THIS

	std::string			realFilePath = _getAbsoluteURIPath(filePath);
	_tryToIndex(realFilePath);

	ss << "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " << s.length() << "\n\n" << s;
	return (ss.str());
}

std::string	CustomSocket::_DELETE(std::string filePath, std::string body, Location *loc)
{
	std::stringstream 	ss;
	std::ifstream		ifs;
	std::string 		s = "DELETE\tat " + filePath + "\nbody:\n" + body;
	
	std::string			realFilePath = _getAbsoluteURIPath(filePath);
	_tryToIndex(realFilePath);
	
	loc = (Location*)loc; // REMOVE THIS

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

std::string CustomSocket::_generateAutoIndex(const std::string path, const std::string relativePath)
{
	DIR 				*dir;
	struct dirent 		*ent;
	std::stringstream	ss;
	std::stringstream   content;

	dir = opendir(path.c_str());
	if (dir == NULL)
	{
		ss << "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 0\n\n"; // TODO: Fit to HTTP norms
		return (ss.str());
	}

	ss << "<!DOCTYPE html><html><head><title>Index of " << relativePath << "</title></head><body><h1>Index of " << relativePath << "</h1><ul>";
	
	while ((ent = readdir(dir)))
	{
		ss << "<li><a href=\"" << relativePath << ent->d_name << "\">" << ent->d_name << "</a></li>";
	}
	closedir(dir);

	ss << "</ul></body></html>";
	content << "HTTP/1.1 200 OK" << "\nContent-Type: text/html" << "\nContent-Length: " << ss.str().length() << "\n\n" << ss.str();

	return (content.str());
}

std::string	CustomSocket::_generateFileContent(const std::string realFilePath)
{
	std::ifstream		ifs;
	std::stringstream	content;

	ifs.open(realFilePath.c_str());
	if (!ifs.is_open())
	{
		if (access(realFilePath.c_str(), F_OK) != 0)
			content << "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 0\n\n"; // TODO: Fit to HTTP norms
		else if (access(realFilePath.c_str(), R_OK) != 0)
			content << "HTTP/1.1 403 Forbidden\nContent-Type: text/plain\nContent-Length: 0\n\n"; // TODO: Fit to HTTP norms
	}
	else
	{
		std::stringstream	buff;
		buff << ifs.rdbuf();
		ifs.close();

		std::string ext = realFilePath.substr(realFilePath.find_last_of(".") + 1);
		bool isImage = (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif");
		std::string contentType = (isImage ? "image/" : "text/") + ext;

		content << "HTTP/1.1 200 OK" << "\nContent-Type: " << contentType << "\nContent-Length: " << buff.str().length() << "\n\n" << buff.str();
	}

	return (content.str());
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

size_t CustomSocket::_isMethodAllowed(const std::string reqType, std::vector<std::string> allowedMethods)

{
	size_t	code = 200;

	if (allowedMethods.size() == 0)
	{
		code = 200;
	}
	else
	{
		for (size_t i = 0; i < allowedMethods.size(); i++)
		{
			if (reqType == allowedMethods[i])
			{
				code = 200;
				break ;
			}
		}
	}
	return (code);
}

size_t CustomSocket::_isContentLengthValid(std::string reqType, std::map<std::string, std::string> headers, long long int maxBodySize)
{
	size_t	code = 200;

	if (headers.find("Content-Length") == headers.end())
	{
		if (reqType == "GET")
			code = 200;
		else
			code = 411;
	}
	else
	{
		try
		{
			if (std::stoi(headers["Content-Length"]) > maxBodySize)
				code = 413;
		}
		catch (const std::exception& e)
		{
			code = 400;
		}
	}
	return (code);
}

void CustomSocket::_generateError(size_t code, std::string &output)
{
	switch (code)
	{
		case 413:
			output = "HTTP/1.1 413 Request Entity Too Large\nContent-Type: text/plain\nContent-Length: 0\n\n";
			break ;
		case 405:
			output = "HTTP/1.1 405 Method Not Allowed\nContent-Type: text/plain\nContent-Length: 0\n\n";
			break ;
		case 411:
			output = "HTTP/1.1 411 Length Required\nContent-Type: text/plain\nContent-Length: 0\n\n";
			break ;
		case 400:
			output = "HTTP/1.1 400 Bad Request\nContent-Type: text/plain\nContent-Length: 0\n\n";
			break ;
		default:
			output = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 9\n\nUNDEFINED";
			break ;
	}
}
