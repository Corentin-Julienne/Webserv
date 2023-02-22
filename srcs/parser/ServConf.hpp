/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConf.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 15:54:49 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/22 14:32:53 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		
	public:

		int											_port;
		std::string									_ip_address;
		std::vector< std::string >					_server_name;
		std::vector< std::vector<std::string> >		_error_pages; // could have more than one
		long long int								_client_max_body_size;
		// used mostly for routes (location blocks, but can apply through everything using inheritance)
		std::vector<std::string>					_allowed_http_methods;
		std::vector< std::vector<std::string> >		_rewrite; // could have more than one
		std::string									_root;
		bool										_autoindex;
		std::string 								_index;
		// should add a cgi there (or not ?)
		std::vector< std::string >					_cgi; // to setup later	
		// handle location blocks
		struct Location								*_loc;
};
