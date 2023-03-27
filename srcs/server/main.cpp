/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:17:08 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/27 11:52:00 by spider-ma        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CustomSocket.hpp"
#include "../parser/Parser.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./webserv <config_file>" << std::endl;
		return (1);
	}

	try
	{
		std::cout << "=== Starting server... ===" << std::endl;

		Parser						configParser(argv[1]);
		int							kq = kqueue();
		std::vector<CustomSocket *>	sockets;

		Parser::servers_array servers = configParser.getServers();

		for (Parser::servers_array::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			CustomSocket	*serverSocket = new CustomSocket(*it, kq);
			sockets.push_back(serverSocket);
		}

		std::cout << "=== Server successfully initialized ===" << std::endl;

		while (true)
		{
			struct kevent	events[1000];
			struct kevent	new_event;
			int				nevents = kevent(kq, NULL, 0, events, 1000, NULL);
			if (nevents < 0)
				continue;
			for (int i = 0; i < nevents; ++i)
			{
				if (!events[i].udata)
					continue ;
				CustomSocket	*socket = reinterpret_cast<CustomSocket *>(events[i].udata);
				if (events[i].filter == EVFILT_READ && events[i].ident == (uintptr_t)socket->getSocketFd())
				{
					socket->acceptConnection(); // handle error
					std::cout << "[" << socket->getPort() << "]\t" \
						<< "Connection accepted\n";
				}
				else if (events[i].filter == EVFILT_READ)
				{
					std::cout << "[" << socket->getPort() << "]\t" \
						<< "New read event\n";
					std::cout << "++++++++++++++++++++++++++++\n";
					std::string	output = socket->read(events[i].ident);
					std::cout << "++++++++++++++++++++++++++++\n\n";
					socket->setOutput(output);
					EV_SET(&new_event, events[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, socket);
					kevent(kq, &new_event, 1, NULL, 0, NULL);
				}
				else if (events[i].filter == EVFILT_WRITE)
				{
					std::cout << "[" << socket->getPort() << "]\t" \
						<< "Writing response\n";
					socket->write(events[i].ident, socket->getOutput());
					socket->closeSocket(events[i].ident);
					std::cout << "[" << socket->getPort() << "]\t" \
						<< "Connection closed\n";
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
