/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiLauncher.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 20:48:18 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/02 10:44:56 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_LAUNCHER_HPP
# define CGI_LAUNCHER_HPP

#include <iostream>
#include <string>
#include <map>
#include <unistd.h>

class cgiLauncher
{
	public:
	
		cgiLauncher(void);
		~cgiLauncher();
		cgiLauncher(const cgiLauncher& original);
		
		cgiLauncher& operator=(const cgiLauncher& original);

	private:

		void			_initEnv(void);
		void			_convStrToSplit(void);
		std::string		_execCgi(void);

	private:
	
		std::map<std::string, std::string>			_env; // env variables
		char										**_char_env;
		std::string									_output;
};

#endif
