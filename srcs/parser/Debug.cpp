/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/03 13:24:05 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/08 17:45:45 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

// set of colors to enhance results visibility and ergonomy
#define YELLOW				"\033[1;33m"
#define BLUE				"\033[1;34m"
#define RED					"\033[1;31m"
#define GREEN				"\033[1;32m"
#define RESET				"\033[0m"

/* contains debugging functions displaying the struct ServConf (for dummy testing structures and for real
structure to assess parsing) */

void	Parser::displayDummyParser(void)
{
	std::size_t		i = 0;
	
	std::cout << "displaying parsing server by server" << std::endl << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	
	while (i < this->_servers.size())
	{
		std::cout << "displaying server " << (i + 1) << " :" << std::endl << std::endl;

		std::cout << "port = " << this->_servers[i]._port << std::endl;
		std::cout << "default_server = " << std::boolalpha << this->_servers[i]._default_server << std::endl;
		
		std::cout << "----------------------------------------------" << std::endl;
		std::cout << "Displaying server_names below :" << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._server_name.size(); j++)
			std::cout << "display server_name arg num : " << this->_servers[i]._server_name[j] << std::endl;
		std::cout << "----------------------------------------------" << std::endl;
		std::cout << "displaying error_pages directive below :" << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._error_pages.size(); j++)
		{
			std::cout << "set of directives number " << (j + 1) << " : ";
			for (std::size_t k = 0; k < this->_servers[i]._error_pages[j].size(); k++)
				std::cout << std::endl << "=> error page arg " << k << " = " << this->_servers[i]._error_pages[j][k] << std::endl;
			std::cout << "----------------------------------------------" << std::endl;
		}
		std::cout << "client_max_body_size = " << this->_servers[i]._client_max_body_size << std::endl;
		
		std::cout << "----------------------------------------------" << std::endl;
		std::cout << "displaying allowed_http_methods below:" << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._allowed_http_methods.size(); j++)
			std::cout << "http methods num " << j << " = " << this->_servers[i]._allowed_http_methods[j] << std::endl;			
		std::cout << "----------------------------------------------" << std::endl;
			
		std::cout << "displaying root = " << this->_servers[i]._root << std::endl;
		std::cout << "displaying autoindex = " << this->_servers[i]._autoindex << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._index.size(); j++)
			std::cout << "displaying index = " << this->_servers[i]._index[j] << std::endl;
		
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;

		std::cout << "displaying every location in Location format" << std::endl << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl;

		for (std::size_t j = 0; j < this->_servers[i]._locs.size(); j++)
			this->displayLocation(this->_servers[i]._locs[j], j); // debug that		
		
		std::cout << std::endl << std::endl;
		i++;	
	}
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << "server display ended" << std::endl;
}

Parser::Parser(void) // to be used only with fake parsing infos
{
	/* first server */
	this->_servers.push_back(ServConf()); // add a first server
	
	this->_servers[0]._port = 80;
	this->_servers[0]._default_server = true;

	//server_name
	std::string					server_name("cjulienn@42.fr");

	this->_servers[0]._server_name.push_back(server_name);
	// error pages setup
	std::vector<std::string>	err_page_vect;
	std::string					err_page("err.log");
	
	this->_servers[0]._error_pages.push_back(err_page_vect);
	this->_servers[0]._error_pages[0].push_back(err_page);
	// client_max_body_size
	this->_servers[0]._client_max_body_size = 1000000;
	this->_servers[0]._autoindex = false;
	this->_servers[0]._index.push_back("index.html");
	this->_servers[0]._root = "www/html";

	// allowed https methods
	this->_servers[0]._allowed_http_methods.push_back("GET");
	
	// add a location block	
	Location			loc;
	
	this->_servers[0]._locs.push_back(loc);

	this->_servers[0]._locs[0]._error_pages = this->_servers[0]._error_pages;
	this->_servers[0]._locs[0]._client_max_body_size = this->_servers[0]._client_max_body_size;
	this->_servers[0]._locs[0]._autoindex = this->_servers[0]._autoindex;
	this->_servers[0]._locs[0]._index = this->_servers[0]._index;
	this->_servers[0]._locs[0]._root = this->_servers[0]._root;
	this->_servers[0]._locs[0]._allowed_http_methods = this->_servers[0]._allowed_http_methods;
	this->_servers[0]._locs[0]._url = "/";
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
	std::cout << "display url = " << loc._url << std::endl;
}
