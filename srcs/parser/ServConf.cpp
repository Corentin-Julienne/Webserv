/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 15:54:52 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/04 15:34:26 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServConf.hpp"

ServConf::ServConf(void) : _ip_address(std::string("0.0.0.0")), _server_name(std::vector<std::string>()),
_error_pages(std::vector< std::vector<std::string> >()), _allowed_http_methods(std::vector<std::string>()),
_rewrite(std::vector< std::vector<std::string> >()), _root(std::string()), _index(std::vector<std::string>()),
_cgi(std::vector<std::string>()), _locs(std::vector<Location>())
{
	this->_port = 80;
	this->_default_server = false;
	this->_client_max_body_size = 1000000;
	this->_autoindex = false;
}

ServConf::~ServConf() {}

ServConf::ServConf(const ServConf& original) : _port(original._port), _ip_address(original._ip_address),
_default_server(original._default_server), _server_name(original._server_name), _error_pages(original._error_pages),
_client_max_body_size(original._client_max_body_size), _allowed_http_methods(original._allowed_http_methods),
_rewrite(original._rewrite), _root(original._root), _autoindex(original._autoindex), _index(original._index),
_cgi(original._cgi), _locs(original._locs) {}

ServConf&	ServConf::operator=(const ServConf& original)
{
	if (this != &original)
	{
		this->_port = original._port;
		this->_ip_address = original._ip_address;
		this->_default_server = original._default_server;
		this->_server_name = original._server_name;
		this->_error_pages = original._error_pages;
		this->_client_max_body_size = original._client_max_body_size;
		this->_allowed_http_methods = original._allowed_http_methods;
		this->_rewrite = original._rewrite;
		this->_root = original._root;
		this->_autoindex = original._autoindex;
		this->_index = original._index;
		this->_cgi = original._cgi;
		this->_locs = original._locs;
	}
	return *this;
}
