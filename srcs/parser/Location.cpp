/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 16:20:39 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/09 13:21:25 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(void) : _error_pages(std::vector< std::vector<std::string> >()),
_allowed_http_methods(std::vector<std::string>()), _root(std::string()),
_index(std::vector<std::string>()), _return(std::vector<std::string>()), _url(std::string())
{
	this->_client_max_body_size = 1000000;
	this->_autoindex = false;
}

Location::~Location() {}

Location::Location(const Location& original) : _error_pages(original._error_pages), 
_client_max_body_size(original._client_max_body_size), _allowed_http_methods(original._allowed_http_methods),
_root(original._root), _autoindex(original._autoindex), _index(original._index), 
_return(original._return), _url(original._url) {}

Location&	Location::operator=(const Location& original)
{
	if (this != &original)
	{
		this->_error_pages = original._error_pages;
		this->_client_max_body_size = original._client_max_body_size;	
		this->_allowed_http_methods = original._allowed_http_methods;
		this->_root = original._root;
		this->_autoindex = original._autoindex;
		this->_index = original._index;
		this->_return = original._return;
		this->_url = original._url;
	}
	return *this;
}
