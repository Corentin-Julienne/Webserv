/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:56 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/28 12:58:42 by spider-ma        ###   ########.fr       */
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
}

CustomSocket::~CustomSocket() 
{
	this->_closeSocket(this->_socket_fd);
}

// main function to start a socket
void	CustomSocket::startServer(void)
{
	ssize_t			valret;
	int				errret;
	struct pollfd	pfd;
	std::string		output = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	
	while (true)
	{
		std::cout << "+++++++++ Waiting for a connection ++++++++" << std::endl;
		this->_acceptConnection(); // use accept to wait for a connection
		pfd.fd = this->_new_socket_fd;

		// read and write procedure
		pfd.events = POLLIN;
		errret = poll(&pfd, 1, -1); // is the socket ready to be read?
		char	buffer[1024]; // create a buffer to be used by read
		if (errret != -1 && pfd.revents == POLLIN) // if case might be useless since read should fail if revents is not POLLIN
			valret = recv(this->_new_socket_fd, buffer, 1024, MSG_TRUNC | MSG_DONTWAIT); // manage case when len > 1024
		if (errret == -1 || pfd.revents != POLLIN || valret < 0)
		{
			std::cerr << "read operation: failure" << std::endl;
			exit(EXIT_FAILURE);
			// handle error there
		}
		std::cout << buffer << std::endl; // print buffer content in terminal, to get debug stuff

		std::string	buff = buffer;
		size_t	start = buff.find(' ');
		size_t	end = buff.find(' ', start + 1);
		std::string	filePath = buff.substr(start, end - start);
		if (buff.substr(0, 4) == "GET ")
			output = this->_GET(filePath);
		else if (buff.substr(0, 5) == "POST ")
			output = this->_POST();
		else if (buff.substr(0, 7) == "DELETE ")
			output = this->_DELETE();
		else
			output = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 9\n\nUNDEFINED";

		pfd.events = POLLOUT;
		errret = poll(&pfd, 1, -1); // is the socket ready for writing?
		if (errret != -1 && pfd.revents == POLLOUT)
			valret = send(this->_new_socket_fd, output.c_str(), output.length(), MSG_DONTWAIT);
		else
		{
			std::cerr << "write operation: failure" << std::endl;
			exit(EXIT_FAILURE);
			// handle error here
		}

		std::cout << "++++++++ Message has been sent ++++++++" << std::endl;

		// suppress the new socket
		this->_closeSocket(this->_new_socket_fd);
	}
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

void	CustomSocket::_acceptConnection(void)
{
	socklen_t socketLen = sizeof(this->_sockaddr);
	if ((this->_new_socket_fd = accept(this->_socket_fd, (struct sockaddr *)&this->_sockaddr, &socketLen)) < 0)
	{
		std::cerr << "accept operation : failure" << std::endl;
		exit(EXIT_FAILURE);
		// handle error here
	}
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
	filePath.erase(0, 2);
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

std::string	CustomSocket::_POST()
{
	return ("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 4\n\nPOST");
}

std::string	CustomSocket::_DELETE()
{
	return ("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 6\n\nDELETE");
}
