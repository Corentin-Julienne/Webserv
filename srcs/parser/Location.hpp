/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 16:20:37 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/10 18:49:54 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

// check if everything is necessary
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <cctype>

struct Location
{
	public:

		Location(void);
		~Location();
		Location(const Location& original);
		Location&	operator=(const Location &original);

	public:

		// can be different from main server scope
		std::vector< std::vector<std::string> >		_error_pages;
		long long int								_client_max_body_size;
		std::vector<std::string>					_allowed_http_methods;
		std::string									_root;
		bool										_autoindex;
		std::vector<std::string> 					_index;
		std::pair<int, std::string>					_return;
		std::vector<std::string>					_cgi;
		// specific to Location
		std::string									_url;
};

#endif
