/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConf.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 15:54:49 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/25 13:05:18 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVCONF_HPP
# define SERVCONF_HPP

#include "./Location.hpp"
// check if everything is necessary
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <cctype>

struct ServConf
{
	// this is a struct so everything is public and accessible
	public:

		ServConf(void);
		~ServConf();
		ServConf(const ServConf &original);
		ServConf&	operator=(const ServConf &original);
		
	public:

		int											_port;
		std::string									_ip_address;
		bool 										_default_server;
		std::vector< std::string >					_server_name;
		std::vector< std::vector<std::string> >		_error_pages;
		long long int								_client_max_body_size;
		// used mostly for routes (location blocks, but can apply through everything using inheritance)
		std::vector<std::string>					_allowed_http_methods;
		std::vector< std::vector<std::string> >		_rewrite;
		std::string									_root;
		bool										_autoindex;
		std::vector<std::string> 					_index;
		std::vector<std::string>					_cgi; // to setup later	
		// handle location blocks
		std::vector<Location>						_locs;
};

#endif
