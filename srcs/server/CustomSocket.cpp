/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CustomSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:27:56 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/19 12:33:58 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./CustomSocket.hpp"

CustomSocket::CustomSocket(void) : _domain(AF_INET), _type(SOCK_STREAM), _protocol(0), _port(8080),
_backlog(10), _new_socket_fd(-1)
{
	this->_socket_fd = socket(_domain, _type, _protocol);
	if (this->_socket_fd < 0)
		; // add function to handle errors
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
	ssize_t		valread;
	std::string	output("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");
	
	while (true)
	{
		std::cout << "+++++++++ Waiting for a connection ++++++++" << std::endl;
		this->_acceptConnection(); // use accept to wait for a connection

		// read and write procedure
		char	buffer[1024]; // create a buffer to be used by read
		valread = read(this->_new_socket_fd, buffer, 1024);
		if (valread < 0)
		{
			std::cerr << "read operation : failure" << std::endl;
			exit(EXIT_FAILURE);
			// handle error there
		}
		std::cout << buffer << std::endl; // print buffer content in terminal, to get debug stuff
		write(this->_new_socket_fd, output.c_str(), output.size());
		// suppress the new socket
		std::cout << "++++++++ Message has been sent ++++++++" << std::endl;
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
	if ((this->_new_socket_fd = accept(this->_socket_fd, (struct sockaddr *)&this->_sockaddr,
		(socklen_t*)&this->_sockaddr)) < 0)
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
