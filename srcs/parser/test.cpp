/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 11:40:09 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/17 01:23:41 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Location.hpp"
#include "ServConf.hpp"

#include <iostream>

// void	kill_the_leaks(void)
// {
// 	system("leaks webserv");
// }

int main(int argc, char **argv)
{	
	//atexit(kill_the_leaks);
	
	std::cout << "Testing parsing process" << std::endl;
	if (argc != 2)
	{
		std::cerr << "add a config file located at ./srcs/parser/test.conf" << std::endl;
		return (1);
	}
	try
	{
		Parser		test_parsing(argv[1]);

		test_parsing.displayParsing();
		std::cout << "ending parsing test" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		std::cout << "parsing tests inconclusive or wrong syntax in conf file" << std::endl;
	}
	return (0);
}
