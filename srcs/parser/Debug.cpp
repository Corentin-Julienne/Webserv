/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/03 13:24:05 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/03 13:26:58 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

/* contains debugging functions displaying the struct ServConf (for dummy testing structures and for real
structure to assess parsing) */

Parser::Parser(void) // to be used only with fake parsing infos
{
	/* first server */
	this->_servers.push_back(ServConf()); // add a first server
	
	this->_servers[0]._port = 80;
	this->_servers[0]._ip_address = "0.0.0.0";
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
	this->_servers[0]._cgi.push_back("NOT YET IMPLEMENTED");

	// allowed https methods
	this->_servers[0]._allowed_http_methods.push_back("GET");
	// rewrite
	std::vector<std::string>		rewrite;
	this->_servers[0]._rewrite.push_back(rewrite);
	this->_servers[0]._rewrite[0].push_back("index.html");
	this->_servers[0]._rewrite[0].push_back("index.php");
	
	// add a location block	
	Location			loc;
	
	this->_servers[0]._locs.push_back(loc);

	this->_servers[0]._locs[0]._error_pages = this->_servers[0]._error_pages;
	this->_servers[0]._locs[0]._client_max_body_size = this->_servers[0]._client_max_body_size;
	this->_servers[0]._locs[0]._autoindex = this->_servers[0]._autoindex;
	this->_servers[0]._locs[0]._index = this->_servers[0]._index;
	this->_servers[0]._locs[0]._root = this->_servers[0]._root;
	this->_servers[0]._locs[0]._cgi = this->_servers[0]._cgi;
	this->_servers[0]._locs[0]._rewrite = this->_servers[0]._rewrite;
	this->_servers[0]._locs[0]._allowed_http_methods = this->_servers[0]._allowed_http_methods;
	this->_servers[0]._locs[0]._url = "/";
}

void	Parser::displayParsing(void)
{
	std::size_t		i = 0;
	
	std::cout << "displaying all server block in once in std::string format" << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << "[" << this->_conf_str << "]" << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	
	std::cout << "displaying parsing server by server" << std::endl << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	
	while (i < this->_servers.size())
	{
		std::cout << "displaying server " << (i + 1) << " :" << std::endl;	
		std::cout << "1) displaying server in std::string format" << std::endl;
		std::cout << this->_server_blocks[i] << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;

		std::cout << "2) displaying server in ServConf format" << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;
		std::cout << "port = " << this->_servers[i]._port << std::endl;
		std::cout << "ip address = " << this->_servers[i]._ip_address << std::endl;
		std::cout << "default_server = " << this->_servers[i]._default_server << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._error_pages.size(); j++)
		{
			std::cout << "set of directives number " << j << " : " <<  std::endl;
			for (std::size_t k = 0; k < this->_servers[i]._error_pages[j].size(); k++)
				std::cout << "error page arg " << k << " = " << this->_servers[i]._error_pages[j][k] << std::endl;
		}
		std::cout << "client_max_blody_size = " << this->_servers[i]._client_max_body_size << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._allowed_http_methods.size() ; j++)
			std::cout << "https methods num " << j << " = " << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._rewrite.size(); j++)
		{
			std::cout << "displaying rewrite directive num " << j << std::endl;
			for (std::size_t k = 0; this->_servers[i]._rewrite[j].size(); k++)
				std::cout << "arg num " << k << " = " << this->_servers[i]._rewrite[j][k] << std::endl;
		}
		std::cout << "displaying root = " << this->_servers[i]._root << std::endl;
		std::cout << "displaying autoindex = " << this->_servers[i]._autoindex << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._index.size(); j++)
			std::cout << "displaying index = " << this->_servers[i]._index[j] << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._cgi.size(); j++)
			std::cout << "displaying cgi = " << this->_servers[i]._cgi[j] << std::endl;
		
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;

		std::cout << "3) displaying every location in Location format" << std::endl;
		
		for (std::size_t j = 0; j < this->_servers[i]._locs.size(); j++)
			this->displayLocation(this->_servers[i]._locs[j]);		
		
		std::cout << std::endl << std::endl;
		i++;
	}
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << "server display ended" << std::endl;
}

void	Parser::displayDummyParser(void)
{
	std::size_t		i = 0;
	
	std::cout << "displaying parsing server by server" << std::endl << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	
	while (i < this->_servers.size())
	{
		std::cout << "displaying server " << (i + 1) << " :" << std::endl << std::endl;

		std::cout << "port = " << this->_servers[i]._port << std::endl;
		std::cout << "ip address = " << this->_servers[i]._ip_address << std::endl;
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
		std::cout << "displaying rewrite directives below" << std::endl;	
		
		for (std::size_t j = 0; j < this->_servers[i]._rewrite.size(); j++)
		{
			std::cout << "displaying rewrite directive num " << (j + 1) << std::endl;
			for (std::size_t k = 0; k < this->_servers[i]._rewrite[j].size(); k++)
				std::cout << "arg num " << k << " = " << this->_servers[i]._rewrite[j][k] << std::endl;
		}
		
		std::cout << "----------------------------------------------" << std::endl;		
		std::cout << "displaying root = " << this->_servers[i]._root << std::endl;
		std::cout << "displaying autoindex = " << this->_servers[i]._autoindex << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._index.size(); j++)
			std::cout << "displaying index = " << this->_servers[i]._index[j] << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._cgi.size(); j++)
			std::cout << "displaying cgi = " << this->_servers[i]._cgi[j] << std::endl;
		
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;

		std::cout << "displaying every location in Location format" << std::endl << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl;

		
		for (std::size_t j = 0; j < this->_servers[i]._locs.size(); j++)
			this->displayLocation(this->_servers[i]._locs[j]);		
		
		std::cout << std::endl << std::endl;
		i++;	
	}
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << "server display ended" << std::endl;
}

void	Parser::displayLocation(Location& loc)
{
	std::cout << "---------------------------------------------------------" << std::endl;
	std::cout << "display location directive error_pages" << std::endl << std::endl;
	for (std::size_t i = 0; i < loc._error_pages.size(); i++)
	{
		std::cout << "displaying set of directive num " << i << " :" << std::endl;
		for (std::size_t j = 0; j < loc._error_pages[i].size(); j++)
			std::cout << "error pages arg " << j << " = " << loc._error_pages[i][j] << std::endl;
	}
	std::cout << "---------------------------------------------------------" << std::endl;
	std::cout << "display client_max_body_size = " << loc._client_max_body_size << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl;
	for (std::size_t i = 0; i < loc._allowed_http_methods.size(); i++)
		std::cout << "display allow directive num " << i << " = " << loc._allowed_http_methods[i] << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl;
	for (std::size_t i = 0; i < loc._rewrite.size(); i++)
	{
		std::cout << "display set of rewrite directives num " << i << std::endl;
		for (std::size_t j = 0; j < loc._rewrite[i].size(); j++)
			std::cout << "rewrite arg " << j << " = " << loc._rewrite[i][j] << std::endl;
	}
	std::cout << "---------------------------------------------------------" << std::endl;
	std::cout << "display root = " << loc._root << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl;
	std::cout << "display autoindex = " << loc._autoindex << std::endl;
	for (std::size_t i = 0; i < loc._index.size(); i++)
		std::cout << "display index arg num " << i << " = " << loc._index[i] << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl;
	for (std::size_t i = 0; i < loc._cgi.size(); i++)
		std::cout << "display cgi arg num " << i << " = " << loc._index[i] << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl;
	std::cout << "display url = " << loc._url << std::endl;
}
