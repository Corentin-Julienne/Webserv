/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:56 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/04 18:36:05 by cjulienn         ###   ########.fr       */
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
	// std::cout << "Socket created on port " << _servconf._port << " (http://localhost:" << _servconf._port << "/)" << std::endl;
}

CustomSocket::~CustomSocket() 
{
	// std::cout << "Closing socket" << std::endl;
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
	EV_SET(&kev, this->_socket_fd, EVFILT_READ, EV_ADD, 0, 0, this);
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
	EV_SET(&events[0], this->_new_socket_fd, EVFILT_READ, EV_ADD, 0, 0, this);
	//EV_SET(&events[1], this->_new_socket_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, this);
	kevent(this->_kq, events, 1, NULL, 0, NULL);
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

std::string	CustomSocket::getOutput(int fd)
{
	return this->_outputs[fd];
}

void CustomSocket::clearOutput(int fd)
{
	this->_outputs.erase(fd);
}

void	CustomSocket::setOutput(int fd, std::string output)
{
	this->_outputs[fd] = output;
}

/* extract query string and store it in SocketInfos struct, then cut query string from the uri */
std::string	CustomSocket::_extractQueryString(SocketInfos &infos)
{
	std::size_t		i = 0;
	std::string		query_string = "";
	
	i = infos.uri.find_first_of('?');
	if (i != std::string::npos)
	{
		if (infos.uri.size() > i)
			query_string = infos.uri.substr(i + 1);
		infos.uri = infos.uri.substr(0, infos.uri.size() - query_string.size());
		if (query_string.size() > 0)
			infos.uri.pop_back();
	}
	return (query_string);
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

	SocketInfos		infos;
	std::string		buff = buffer;
	std::string		output;

	this->_parseRequest(buff, infos.reqType, infos.uri, infos.headers, infos.body);
	
	// Add the suffix to the uri if it's a directory
	if (infos.uri.substr(0, 1) != "/")
		infos.uri = "/" + infos.uri;

	/* add relative path_info and query string to infos struct, withdraw query string from uri */
	infos.queryString = this->_extractQueryString(infos);

	Location 	*loc = _getPathLocation(infos.uri);
	size_t		code = _isMethodAllowed(infos.reqType, (loc ? loc->_allowed_http_methods : _servconf._allowed_http_methods));

	if (code == 200)
		code = _isContentLengthValid(infos.reqType, infos.headers, (loc ? loc->_client_max_body_size : _servconf._client_max_body_size));

	if (code == 200)
	{
		if (infos.reqType == "GET")
			output = _GET(infos, loc);
		else if (infos.reqType == "POST")
			output = _POST(infos, loc);
		else if (infos.reqType == "DELETE")
			output = _DELETE(infos, loc);
		else
			output = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 9\n\nUNDEFINED";
	}
	else
		output = _generateError(code, loc);
	
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

std::string	CustomSocket::_GET(SocketInfos &infos, Location *loc)
{
	std::string			ret;
	std::stringstream	content;
	std::string			realFilePath = _getAbsoluteURIPath(infos.uri);

	_tryToIndex(realFilePath);
	
	bool isDirectory = (realFilePath.substr(realFilePath.length() - 1, 1) == "/");
	if (isDirectory)
	{
		if (infos.uri.substr(infos.uri.length() - 1, 1) != "/")
			infos.uri += "/";
		if ((loc ? loc->_autoindex : _servconf._autoindex))
			content << _generateAutoIndex(realFilePath, infos.uri, loc);
		else
			content << _generateError(404, loc);
	}
	else if (realFilePath.size() > 4 && !realFilePath.substr(realFilePath.size() - 4).compare(".php")) // triggers php cgi
	{		
		infos.absoluteURIPath = realFilePath;
		
		cgiLauncher		cgi(infos, *loc, this->_servconf);
		
		content << cgi.exec();
	}
	else
		content << _generateFileContent(realFilePath, loc);

	return (content.str());
}

std::string	CustomSocket::_POST(SocketInfos &infos, Location *loc)
{
	std::stringstream ss;
	std::string s = "POST\tat " + infos.uri + "\nbody:\n" + infos.body;

	loc = (Location*)loc; // REMOVE THIS

	std::string			realFilePath = _getAbsoluteURIPath(infos.uri);
	_tryToIndex(realFilePath);

	ss << "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " << s.length() << "\n\n" << s;
	return (ss.str());
}

std::string	CustomSocket::_DELETE(SocketInfos &infos, Location *loc)
{
	std::stringstream 	ss;
	std::ifstream		ifs;
	std::string 		s = "DELETE\tat " + infos.uri + "\nbody:\n" + infos.body;
	
	std::string			realFilePath = _getAbsoluteURIPath(infos.uri);
	_tryToIndex(realFilePath);
	
	ifs.open(realFilePath.c_str());
	if (ifs.is_open())
	{
		ifs.close();
		std::remove(realFilePath.c_str());
		ss << "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " << s.length() << "\n\n" << s;
	}
	else
	{
		ss << _generateError(404, loc);
	}

	return (ss.str());
}

std::string CustomSocket::_generateAutoIndex(const std::string path, const std::string relativePath, Location *loc)
{
	DIR 				*dir;
	struct dirent 		*ent;
	std::stringstream	ss;
	std::stringstream   content;

	dir = opendir(path.c_str());
	if (dir == NULL)
	{
		ss << _generateError(404, loc);
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

std::string	CustomSocket::_generateFileContent(const std::string realFilePath, Location *loc)
{
	std::ifstream		ifs;
	std::stringstream	content;

	ifs.open(realFilePath.c_str());
	if (!ifs.is_open())
	{
		if (access(realFilePath.c_str(), F_OK) != 0)
			content << _generateError(404, loc);
		else if (access(realFilePath.c_str(), R_OK) != 0)
			content << _generateError(403, loc);
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
		if (uriPath.find(_servconf._locs[i]._url) != std::string::npos && _servconf._locs[i]._url.size() > (location ? location->_url.size() : 0))
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
	size_t	code = 405;

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

std::string CustomSocket::_generateError(size_t code, Location *location)
{
	std::stringstream ss;
	std::vector< std::vector<std::string> > errors;

	if (location)
		errors = location->_error_pages;
	else
		errors = _servconf._error_pages;

	switch (code)
	{
		case 413:
			ss << "HTTP/1.1 413 Request Entity Too Large\nContent-Type: text/plain\nContent-Length: ";
			break ;
		case 405:
			ss << "HTTP/1.1 405 Method Not Allowed\nContent-Type: text/plain\nContent-Length: ";
			break ;
		case 411:
			ss << "HTTP/1.1 411 Length Required\nContent-Type: text/plain\nContent-Length: ";
			break ;
		case 400:
			ss << "HTTP/1.1 400 Bad Request\nContent-Type: text/plain\nContent-Length: ";
			break ;
		case 404:
			ss << "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: ";
			break ;
		case 403:
			ss << "HTTP/1.1 403 Forbidden\nContent-Type: text/plain\nContent-Length: ";
			break ;
		default:
			ss << "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
			break ;
	}

	// List through all errors pages to find the one corresponding to the error code
	std::stringstream errStr;
	errStr << code;
	std::string errCode = errStr.str();

	for (size_t i = 0; i < errors.size(); i++)
	{
		for (size_t j = 0; j < errors[i].size() - 1; j++)
		{
			if (errors[i][j] == errCode)
			{
				std::string	realErrPath = _getAbsoluteURIPath(errors[i][errors[i].size() - 1]);
				std::ifstream ifs(realErrPath.c_str());

				if (ifs.is_open())
				{
					std::stringstream buff;
					buff << ifs.rdbuf();
					ifs.close();
					ss << buff.str().length() << "\n\n" << buff.str();
					return (ss.str());
				}
			}
		}
	}

	ss << "0\n\n";
	return (ss.str());
}