/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/03 13:24:05 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/09 17:27:46 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

// set of colors to enhance results visibility and ergonomy
#define YELLOW				"\033[1;33m"
#define BLUE				"\033[1;34m"
#define RED					"\033[1;31m"
#define GREEN				"\033[1;32m"
#define RESET				"\033[0m"

void	Parser::displayDefaultParsing(void)
{
	if (this->_servers.empty())
	{
		std::cout << "problem : server block non existing" << std::endl;
		return ;
	}
	std::cout << "number of servers = " << this->_servers.size() << std::endl;
	std::cout << "port = " << this->_servers[0]._port << std::endl;
	if (this->_servers[0]._locs.empty())
	{
		std::cout << "problem : no location block" << std::endl;
		return ;
	}
	std::cout << "number of location = " << _servers[0]._locs.size() << std::endl;
	std::cout << "root = " << _servers[0]._locs[0]._root << std::endl;
	std::cout << "index = " << _servers[0]._locs[0]._index[0] << std::endl;
	std::cout << "allow = " << _servers[0]._locs[0]._allowed_http_methods.at(0) << std::endl;
	std::cout << "url = " << _servers[0]._locs[0]._url << std::endl;
	std::cout << "cgi =" << _servers[0]._locs[0]._cgi[0] << " , " << _servers[0]._locs[0]._cgi[1] << std::endl;
}

void	Parser::displayParsing(void)
{
	std::size_t		i = 0;
	
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << BLUE << "displaying all server block in once in std::string format" << RESET << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << "||" << this->_conf_str << "||" << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << BLUE << "displaying parsing server by server" << RESET << std::endl << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	
	while (i < this->_servers.size())
	{
		std::cout << YELLOW << "displaying server " << (i + 1) << " :" << RESET << std::endl;	
		std::cout << GREEN << "1) displaying server in std::string format" << RESET << std::endl;
		std::cout << this->_server_blocks[i] << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;

		std::cout << GREEN << "2) displaying server in ServConf format" << RESET << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;
		std::cout << "port = " << this->_servers[i]._port << std::endl;
		std::cout << "default_server = " << std::boolalpha << this->_servers[i]._default_server << std::endl;
		
		std::cout << "---------------------------------------------------------" << std::endl;
		std::cout << RED << "Displaying content of server_name" << RESET << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._server_name.size(); j++)
			std::cout << "server_name number " << j << " is = |" << this->_servers[i]._server_name[j] << "|" << std::endl;;
		
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;
		std::cout << "displaying error_pages directive below :" << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._error_pages.size(); j++)
		{
			std::cout << RED << "set of directives number " << (j + 1) << " : " << RESET;
			for (std::size_t k = 0; k < this->_servers[i]._error_pages[j].size(); k++)
				std::cout << std::endl << "=> error page arg " << k << " = " << this->_servers[i]._error_pages[j][k] << std::endl;
			std::cout << "----------------------------------------------" << std::endl;
		}
		std::cout << "client_max_body_size = " << this->_servers[i]._client_max_body_size << std::endl;
		std::cout << "----------------------------------------------" << std::endl;
		std::cout << RED << "displaying allowed_http_methods below:" << RESET << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._allowed_http_methods.size(); j++)
			std::cout << "http methods num " << j << " = " << this->_servers[i]._allowed_http_methods[j] << std::endl;			
		std::cout << "----------------------------------------------" << std::endl;
		
		std::cout << "displaying root = " << this->_servers[i]._root << std::endl;
		std::cout << "displaying autoindex = " << this->_servers[i]._autoindex << std::endl;
		std::cout << RED << "displaying index directive : " << RESET << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._index.size(); j++)
			std::cout << "displaying index = " << this->_servers[i]._index[j] << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;
		
		std::cout << RED << "displaying return directive : " << RESET << std::endl;
		if (!this->_servers[i]._return.empty())
		{
			std::cout << "return code = " << this->_servers[i]._return[0] << " and url = " << this->_servers[i]._return[1] 
			<< std::endl;
		}
		std::cout << RED << "dislaying cgi info..." << RESET << std::endl;
		std::cout << "cgi extension == " << this->_servers[i]._cgi[0] << " and cgi_script = " 
		<< this->_servers[i]._cgi[1]  << std::endl;
		std::cout << GREEN << "3) displaying every location in Location format" << RESET << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._locs.size(); j++)
			this->displayLocation(this->_servers[i]._locs[j], j);
		
		std::cout << std::endl << std::endl;
		i++;
	}
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << BLUE << "server display ended" << RESET << std::endl;
}

void	Parser::displayLocation(Location& loc, int loc_index)
{
	std::cout << "---------------------------------------------------------" << std::endl;
	std::cout << YELLOW << "Location number " << loc_index << RESET << std::endl << std::endl;
	
	std::cout << "---------------------------------------------------------" << std::endl;
	std::cout << "display location directive error_pages below:" << std::endl << std::endl;
	for (std::size_t i = 0; i < loc._error_pages.size(); i++)
	{
		std::cout << RED << "displaying set of directive num " << i << " :" << RESET << std::endl;
		for (std::size_t j = 0; j < loc._error_pages[i].size(); j++)
			std::cout << "error pages arg " << j << " = " << loc._error_pages[i][j] << std::endl;
	}
	std::cout << "---------------------------------------------------------" << std::endl;
	std::cout << "client_max_body_size = " << loc._client_max_body_size << std::endl;	
	
	std::cout << "---------------------------------------------------------" << std::endl;
	std::cout << "displaying allow directives below : " << std::endl;
	for (std::size_t i = 0; i < loc._allowed_http_methods.size(); i++)
		std::cout << RED << "display allow directive num " << i << " = " << RESET << loc._allowed_http_methods[i] << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl;
	
	std::cout << "display root = " << loc._root << std::endl;
	
	std::cout << "display autoindex = " << std::boolalpha << loc._autoindex << std::endl;
	
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << RED << "displaying index directive : " << RESET << std::endl;
	for (std::size_t i = 0; i < loc._index.size(); i++)
		std::cout << "display index arg num " << i << " = " << loc._index[i] << std::endl;	
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << RED << "displaying return directive : " << RESET << std::endl;
	if (!loc._return.empty())
		std::cout << "return code == " << loc._return[0] << " and url = |" << loc._return[1] << "|" << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	
	std::cout << "dislaying cgi info..." << std::endl;
	std::cout << "cgi extension == " << loc._cgi[0] << " and cgi_script = " << loc._cgi[1]  << std::endl;
	
	std::cout << "display url = " << loc._url << std::endl;
}
