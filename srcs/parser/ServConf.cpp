/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 15:54:52 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/08 17:56:54 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServConf.hpp"

ServConf::ServConf(void) : _server_name(std::vector<std::string>()),
_error_pages(std::vector< std::vector<std::string> >()), _allowed_http_methods(std::vector<std::string>()),
_root(std::string()), _index(std::vector<std::string>()), _locs(std::vector<Location>())
{
	this->_port = 80;
	this->_default_server = false;
	this->_client_max_body_size = 1000000;
	this->_autoindex = false;
}

ServConf::~ServConf() {}

ServConf::ServConf(const ServConf& original) : _port(original._port),
_default_server(original._default_server), _server_name(original._server_name), _error_pages(original._error_pages),
_client_max_body_size(original._client_max_body_size), _allowed_http_methods(original._allowed_http_methods),
_root(original._root), _autoindex(original._autoindex), _index(original._index), _locs(original._locs) {}

ServConf&	ServConf::operator=(const ServConf& original)
{
	if (this != &original)
	{
		this->_port = original._port;
		this->_default_server = original._default_server;
		this->_server_name = original._server_name;
		this->_error_pages = original._error_pages;
		this->_client_max_body_size = original._client_max_body_size;
		this->_allowed_http_methods = original._allowed_http_methods;
		this->_root = original._root;
		this->_autoindex = original._autoindex;
		this->_index = original._index;
		this->_locs = original._locs;
	}
	return *this;
}
