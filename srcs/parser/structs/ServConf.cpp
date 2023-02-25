/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 15:54:52 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/23 22:40:57 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServConf.hpp"

ServConf::ServConf(void) : _ip_address(std::string("0.0.0.0"))
{
	this->_port = 80; // default nginx value
	this->_client_max_body_size = 1000000; // default nginx value
	this->_autoindex = false; // default nginx value
}

ServConf::~ServConf() {}
