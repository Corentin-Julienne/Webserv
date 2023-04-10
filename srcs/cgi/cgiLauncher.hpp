/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiLauncher.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 20:48:18 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/10 14:23:57 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CGI_LAUNCHER_HPP
# define CGI_LAUNCHER_HPP

#include <iostream>
#include <string>
#include <map>
#include <unistd.h>

#include "../server/SocketInfos.hpp"
#include "../parser/ServConf.hpp"
#include "../parser/Location.hpp"

#define BUFFER_SIZE		1024

class cgiLauncher
{
	public:
	
		cgiLauncher(SocketInfos &infos, ServConf &serv, std::string cgi_path);
		~cgiLauncher();
		cgiLauncher(const cgiLauncher& original);
		
		cgiLauncher& operator=(const cgiLauncher& original);

		int					exec(void);
		const int&			getCode(void) const;
		const std::string&	getOutput(void) const;

	private:

		void			_initEnv(void);
		void			_StrEnvToCStrArray(void);
		char			**_getArgs(std::string path);

		std::string		_numToStr(int num);

		void			_addHeadersToEnv(void);
		std::string		_extractContentType(std::string ouput);
		std::string		_extractCGIHeader(std::string output);
		std::string		_formatOutput(std::string output);
		std::string 	_trimWhitespaces(std::string str);

		/* debug functions */

		void			_printInfos(void);
		void			_printEnv(void);

	private:
	
		cgiLauncher(void);
	
		SocketInfos									_infos;
		ServConf									_serv;
		std::string									_scriptPath;
		std::map<std::string, std::string>			_env; // env variables
		char										**_char_env;
		std::string									_output;
		std::string									_cwd;
		int											_code;
		/* output */
		std::string									_contentType;
		std::string									_output_headers;
};

#endif
