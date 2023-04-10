/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConf.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 15:54:49 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/10 18:50:02 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVCONF_HPP
# define SERVCONF_HPP

#include "Location.hpp"
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
		bool 										_default_server;
		std::vector<std::string>					_server_name;
		std::vector< std::vector<std::string> >		_error_pages;
		long long int								_client_max_body_size;
		// used mostly for routes (location blocks, but can apply through everything using inheritance)
		std::vector<std::string>					_allowed_http_methods;
		std::string									_root;
		bool										_autoindex;
		std::vector<std::string> 					_index;
		std::pair<int, std::string>					_return;
		std::vector<std::string>					_cgi;
		// handle location blocks
		std::vector<Location>						_locs;
};

#endif
