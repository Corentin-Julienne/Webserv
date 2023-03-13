/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:56 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/13 15:24:27 by spider-ma        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./CustomSocket.hpp"

CustomSocket::CustomSocket(void) : _domain(AF_INET), _type(SOCK_STREAM), _protocol(0), _port(8080),
_backlog(10), _new_socket_fd(-1)
{
	this->_socket_fd = socket(_domain, _type, _protocol);
	if (this->_socket_fd < 0) {} // add function to handle errors
	this->_bindSocket();
	this->_enableSocketListening();
	this->_createKq();
}

CustomSocket::~CustomSocket() 
{
	this->_closeSocket(this->_socket_fd);
}

// main function to start a socket
void	CustomSocket::startServer(void)
{
	std::string		output = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	
	while (true)
	{
		std::cout << "+++++++++ Waiting for a connection ++++++++" << std::endl;

		// read and write procedure
		struct kevent	events[1000];
		struct kevent	new_event;
		int				nevents = kevent(this->_kq, NULL, 0, events, 1000, NULL);
		if (nevents < 0)
			continue;
		for (int i = 0; i < nevents; ++i)
		{
			if (events[i].filter == EVFILT_READ && events[i].ident == (uintptr_t)this->_socket_fd)
				this->_acceptConnection();
			else if (events[i].filter == EVFILT_READ)
			{
				output = this->_read(events[i].ident);
				EV_SET(&new_event, events[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, const_cast<char *>(output.c_str()));
				kevent(this->_kq, &new_event, 1, NULL, 0, NULL);
			}
			else if (events[i].filter == EVFILT_WRITE)
			{
				this->_write(events[i].ident, static_cast<char *>(events[i].udata));
				this->_closeSocket(events[i].ident);
			}
		}

		std::cout << "++++++++ Message has been sent ++++++++" << std::endl;
	}
}

std::string	CustomSocket::_read(int fd)
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

void	CustomSocket::_write(int fd, char *output)
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
	this->_sockaddr.sin_port = htons(this->_port);
	
	if (bind(this->_socket_fd, (struct sockaddr *)&this->_sockaddr, sizeof(this->_sockaddr)) < 0)
	{
		std::cerr << "bind operation : failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}
}

void	CustomSocket::_enableSocketListening(void)
{
	if (listen(this->_socket_fd, this->_backlog) < 0)
	{
		std::cerr << "listen operation : failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}
}

void	CustomSocket::_createKq(void)
{
	// don't forget protections
	this->_kq = kqueue();
	struct kevent	kev;
	EV_SET(&kev, this->_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
	kevent(this->_kq, &kev, 1, NULL, 0, NULL);
}

void	CustomSocket::_acceptConnection(void)
{
	socklen_t socketLen = sizeof(this->_sockaddr);
	if ((this->_new_socket_fd = accept(this->_socket_fd, (struct sockaddr *)&this->_sockaddr, &socketLen)) < 0)
	{
		std::cerr << "accept operation : failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error here
	}
	fcntl(this->_new_socket_fd, F_SETFL, O_NONBLOCK);
	struct kevent	events[2];
	EV_SET(&events[0], this->_new_socket_fd, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, 0);
	EV_SET(&events[1], this->_new_socket_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE | EV_ONESHOT, 0, 0, 0);
	kevent(this->_kq, events, 2, NULL, 0, NULL);
}

void	CustomSocket::_closeSocket(int socket_fd)
{
	if (close(socket_fd) < 0)
	{
		std::cerr << "close operation : failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error there
	}
}

#include <sstream>
#include <fstream>
#include <unistd.h>

std::string	CustomSocket::_GET(std::string filePath)
{
	std::string			ret;
	std::ifstream		ifs;
	std::stringstream	content;
	ifs.open(filePath);
	if (!ifs.is_open())
	{
		if (access(filePath.c_str(), F_OK) != 0)
			content << "HTTP/1.1 404 Not Found";
		else if (access(filePath.c_str(), R_OK) != 0)
			content << "HTTP/1.1 403 Forbidden";
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
	std::string s = "POST\tat " + filePath + "\nbody:\n" + body;
	return ("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " + std::to_string(s.length()) + "\n\n" + s); // to_string is C++11
}

std::string	CustomSocket::_DELETE(std::string filePath, std::string body)
{
	std::string s = "DELETE\tat " + filePath + "\nbody:\n" + body;
	return ("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " + std::to_string(s.length()) + "\n\n" + s); // to_string is C++11
}
