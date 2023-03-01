/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 11:40:09 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/01 15:25:31 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Location.hpp"
#include "ServConf.hpp"

#include <iostream>

int main(int argc, char **argv)
{
	(void)argv; // silence warning
	
	std::cout << "Testing parsing process" << std::endl;
	if (argc != 2)
		std::cerr << "add a config file located at ./srcs/parser/test.conf" << std::endl;
	
	Parser		test_parsing;
	
	test_parsing.displayDummyParser();
	std::cout << "ending parsing test" << std::endl;
	return (0);
}
