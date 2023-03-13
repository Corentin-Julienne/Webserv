/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:17:08 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/13 18:08:10 by mpeharpr         ###   ########.fr       */
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
		std::vector<CustomSocket>	sockets;

		Parser::servers_array servers = configParser.getServers();
		Parser::servers_array::iterator it = servers.begin();
		Parser::servers_array::iterator ite = servers.end();

		while (it != ite)
		{
			CustomSocket	serverSocket(*it, kq);
			sockets.push_back(serverSocket);
			it++;
		}

		std::cout << "=== Server successfully initialized ===" << std::endl;

		while (true)
		{
			std::cout << "+++++++++ Waiting for a connection ++++++++" << std::endl;

			struct kevent	events[1000];
			struct kevent	new_event;
			int				nevents = kevent(kq, NULL, 0, events, 1000, NULL);
			if (nevents < 0)
				continue;
			for (int i = 0; i < nevents; ++i)
			{
				CustomSocket	socket = *reinterpret_cast<CustomSocket *>(events[i].udata);
				if (events[i].filter == EVFILT_READ && events[i].ident == (uintptr_t)socket.getSocketFd())
					socket.acceptConnection();
				else if (events[i].filter == EVFILT_READ)
				{
					std::string	output = socket.read(events[i].ident);
					socket.setOutput(const_cast<char *>(output.c_str()));
					EV_SET(&new_event, events[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, &socket);
					kevent(kq, &new_event, 1, NULL, 0, NULL);
				}
				else if (events[i].filter == EVFILT_WRITE)
				{
					socket.write(events[i].ident, static_cast<char *>(socket.getOutput()));
					socket.closeSocket(events[i].ident);
				}
			}
	
			std::cout << "++++++++ Message has been sent ++++++++" << std::endl;
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
