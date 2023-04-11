/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:17:08 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/11 14:06:52 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "CustomSocket.hpp"

size_t fileExists(const std::string realFilePath)
{
	std::ifstream	ifs;
	
	ifs.open(realFilePath.c_str());
	if (!ifs.is_open())
	{
		if (access(realFilePath.c_str(), F_OK) != 0)
			return (404);
		else if (access(realFilePath.c_str(), R_OK) != 0)
			return (403);
	}
	return (200);
}

bool	isDirectory(const std::string &path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false;
	return S_ISDIR(statbuf.st_mode);
}

void	call_error(std::string failing_call, bool exit_process)
{
	(void)failing_call;
	if (exit_process)
		exit(EXIT_FAILURE);
}

int	main(int argc, char **argv)
{
	if (argc < 1 || argc > 2)
	{
		std::cerr << "Usage: ./webserv [config_file]" << std::endl;
		return (1);
	}

	std::string configPath;
	std::string defaultPath = "config/default.conf";

	if (argc == 1)
	{
		size_t code = fileExists(defaultPath);
		if (code == 404)
		{
			std::cerr << "Default file does not exists anymore: please specify a config_file!" << std::endl;
			return (1);
		}
		else if (code == 403)
		{
			std::cerr << "Default file is unreadable: please specify a config_file!" << std::endl;
			return (1);
		}

		configPath = defaultPath;
	}
	else
		configPath = argv[1];

	try
	{
		std::cout << "=== Starting server... ===" << std::endl;

		std::vector<CustomSocket *>	sockets;
		Parser		configParser = Parser(configPath.c_str());
		
		int	kq = kqueue();
		if (kq == -1)
			call_error("kqueue", true);

		Parser::servers_array servers = configParser.getServers();
		Parser::servers_array default_servers;

		// Sort server without editing the current order
		for (Parser::servers_array::iterator it = servers.begin(); it != servers.end();)
		{
			if (it->_default_server)
			{
				default_servers.push_back(*it);
				it = servers.erase(it);
			} 
			else
				++it;
		}
		servers.insert(servers.begin(), default_servers.begin(), default_servers.end());

		// Create the servers
		for (Parser::servers_array::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			bool shouldStart = true;

			for (size_t si = 0; si < sockets.size(); si++)
			{
				if (sockets[si]->getPort() == it->_port)
				{
					std::cout << "-> Server on http://localhost:" << it->_port << "/ skipped since another server is using the same port" << std::endl;
					shouldStart = false;
					break;
				}
			}

			if (!shouldStart)
				continue;
			
			CustomSocket	*serverSocket = new CustomSocket(*it, kq);
			sockets.push_back(serverSocket);

			std::cout << "-> Server started: http://localhost:" << it->_port << "/" << std::endl;
		}

		std::cout << "=== Server successfully initialized ===" << std::endl;

		while (true)
		{
			struct kevent	events[1000];
			struct kevent	new_event[2];
			int				nevents = kevent(kq, NULL, 0, events, 1000, NULL);

			if (nevents < 0)
			{
				call_error("kevent", false);
				continue;
			}

			for (int i = 0; i < nevents; ++i)
			{
				if (!events[i].udata)
					continue ;
				CustomSocket	*socket = reinterpret_cast<CustomSocket *>(events[i].udata);
				if (events[i].filter == EVFILT_READ && events[i].ident == (uintptr_t)socket->getSocketFd())
					socket->acceptConnection();
				else if (events[i].filter == EVFILT_READ)
				{
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
					socket->write(events[i].ident);
					EV_SET(&new_event[0], events[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, socket);
					if (kevent(kq, new_event, 1, NULL, 0, NULL) == -1)
						call_error("kevent", false);
					socket->closeSocket(events[i].ident);
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
