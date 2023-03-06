/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:17:08 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/06 06:07:25 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CustomSocket.hpp"
#include "../parser/Parser.hpp"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./webserv <config_file>" << std::endl;
		return (1);
	}

	try
	{
		std::cout << "Starting server..." << std::endl;
		
		Parser			configParser(argv[1]);

		Parser::servers_array servers = configParser.getServers();
		Parser::servers_array::iterator it = servers.begin(); 
		Parser::servers_array::iterator ite = servers.end();

		while (it != ite)
		{
			CustomSocket	serverSocket(it->_port);
			serverSocket._startServer();

			// Should be in a thread since startServer is blocking with an infinite loop

			it++;
		}

		std::cout << "Server successfully initialized!" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "Stopping server! An error has occured: " << e.what() << std::endl;
		return (1);
	}
	
	return (0);
}
