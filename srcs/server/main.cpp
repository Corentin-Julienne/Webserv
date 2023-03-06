/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 12:17:08 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/06 03:48:14 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./CustomSocket.hpp"

CustomSocket	*createdSocket;

void signal_handler(int signum)
{
	std::cout << "Interrupt signal (" << signum << ") received.\n";
	if (createdSocket)
		createdSocket->~CustomSocket();
	exit(signum);
}

int main(void)
{
	createdSocket = NULL;
	signal(SIGINT, signal_handler);	

	CustomSocket	test_server;
	createdSocket = &test_server;

	test_server.startServer();
	return (0);
}
