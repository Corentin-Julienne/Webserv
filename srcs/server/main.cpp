/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:17:08 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/08 18:29:31 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "CustomSocket.hpp"

bool	isDirectory(const std::string &path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false;
	return S_ISDIR(statbuf.st_mode);
}

void	call_error(std::string failing_call, bool exit_process)
{
	std::cerr << failing_call << ": " << strerror(errno) << std::endl;
	if (exit_process)
		exit(EXIT_FAILURE);
}

int	main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Usage: ./webserv <config_file> or ./webserv (with default params)" << std::endl;
		return (1);
	}
	try
	{
		std::cout << "=== Starting server... ===" << std::endl;

		std::vector<CustomSocket *>	sockets;

		Parser		configParser = (argc == 2) ? Parser(argv[1]) : Parser();
		
		int							kq = kqueue();
		if (kq == -1)
			call_error("kqueue", true);

		Parser::servers_array servers = configParser.getServers();

		for (Parser::servers_array::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			CustomSocket	*serverSocket = new CustomSocket(*it, kq);
			sockets.push_back(serverSocket);

			std::cout << "-> Server started: http://" << "0.0.0.0 :" << it->_port << std::endl;
		}

		std::cout << "=== Server successfully initialized ===" << std::endl;

		while (true)
		{
			struct kevent	events[1000];
			struct kevent	new_event[2];
			int				nevents = kevent(kq, NULL, 0, events, 1000, NULL);

			if (nevents < 0)
				continue;

			for (int i = 0; i < nevents; ++i)
			{
				if (!events[i].udata)
				{
					std::cout << "!!! ERROR !!!" << std::endl;
					continue ;
				}
				CustomSocket	*socket = reinterpret_cast<CustomSocket *>(events[i].udata);
				if (events[i].filter == EVFILT_READ && events[i].ident == (uintptr_t)socket->getSocketFd())
				{
					socket->acceptConnection(); // handle error
					// std::cout << "[" << socket->getPort() << "]\t" \
						// << "Connection accepted\n";
				}
				else if (events[i].filter == EVFILT_READ)
				{
					// std::cout << "[" << socket->getPort() << "]\t" \
						// << "New read event\n";
					socket->read(events[i].ident);
					EV_SET(&new_event[0], events[i].ident, EVFILT_READ, EV_DELETE, 0, 0, socket);
					EV_SET(&new_event[1], events[i].ident, EVFILT_WRITE, EV_ADD, 0, 0, socket);
					if (kevent(kq, new_event, 2, NULL, 0, NULL) == -1)
					{
						call_error("kevent", false);
						socket->closeSocket(events[i].ident);
					}
				}
				else if (events[i].filter == EVFILT_WRITE)
				{
					// std::cout << "[" << socket->getPort() << "]\t" \
						// << "Writing response\n";
						
					socket->write(events[i].ident);
					EV_SET(&new_event[0], events[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, socket);
					if (kevent(kq, new_event, 1, NULL, 0, NULL) == -1)
						call_error("kevent", false);
					socket->closeSocket(events[i].ident);

					// std::cout << "[" << socket->getPort() << "]\t"
						// << "Connection closed\n";
				}
			}
		}

		// Delete sockets
		std::vector<CustomSocket *>::iterator it2 = sockets.begin();
		std::vector<CustomSocket *>::iterator it2e = sockets.end();
		
		while (it2 != it2e)
		{
			delete *it2;
			it2++;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "Stopping server! An error has occured: " << e.what() << std::endl;
		return (1);
	}

	std::cout << "=== Webserv ended successfully ===" << std::endl;
	return (0);
}
