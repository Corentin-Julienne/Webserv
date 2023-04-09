/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 08:10:40 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/09 17:02:31 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

#include "ServConf.hpp"
#include "Location.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <limits>
#include <climits>
#include <stdlib.h>
#include <unistd.h>

#define BAD_INSTR			0
#define LOCATION			1
#define LISTEN				2
#define SERV_NAME			3
#define ERR_PAGE			4
#define CLIENT_BODY_SIZE	5
#define ALLOW_HTTP_METHOD	6
#define ROOT				7
#define AUTOINDEX			8
#define INDEX				9
#define RETURN				10
#define CGI					11

#define MAX_PORT			65535

class Parser
{
	public:
	
		typedef std::vector<ServConf>		servers_array;

		Parser(void);
		Parser(char *config_file);
		~Parser();
		Parser(const Parser& original);

		Parser& operator=(const Parser& original);

		const servers_array	getServers(void) const;

	private:
	
		/* Parser.cpp */
		
		// ops on conf file
		void			_processFile(void);
		void			_ifstreamToStr(void);
		// separating std::string on different server blocks
		void			_iterateThroughStr(void);
		int				_splitServerBlock(int i);
		bool			_isBlockSyntaxValid(void);
		bool			_areBracketsPopulated(std::string content);
		std::string		_extractBracketsBlock(std::string content);
		bool			_isServerBlockValid(std::string substr);
		
		/* ProcessBlocks.cpp */	
		void			_processBlock(std::string block, int server_index, bool is_loc = false);
		std::size_t		_processLocationBlock(std::string directive, int server_index);
		void			_enforceInheritance(Location& loc, int server_index);
		bool			_isLocationBlockValid(std::string block);
		bool			_isDirectiveValid(std::string directive);
		bool			_isThereEnoughInfo(void);

		/* ParseDirectives.cpp */
		void			_processListenDirective(std::string directive, int serv_idx, 
							int arg_num, bool is_loc = false);
		void			_processServerNameDirective(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);
		void			_processErrorPageDirective(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);
		void			_processBodySizeDirective(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);
		void			_processAllowDirective(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);
		void			_processRewriteDirective(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);
		void			_processRootDirective(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);
		void			_processAutoindexDirective(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);
		void			_processIndexDirective(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);
		void			_processReturnDirective(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);
		void			_processCGI(std::string directive, int serv_idx,
							int arg_num, bool is_loc = false);

		/* Utils.cpp */
		std::vector<std::string>	_cutArgs(std::string directive, char delim);
		// bool						_isIpValid(std::string ip);
		int							_rtnInstructionType(std::string directive);
		int							_dispatchInstructionProcessing(int type, std::string directive,
										int serv_idx, bool is_loc = false);

	public:
		
		/* test features */
		void						displayParsing(void);	
		void						displayLocation(Location& loc, int loc_index);
		void						displayDefaultParsing(void);
		
	private:
		
		std::ifstream					_conf_file;
		std::string						_conf_str;
		std::vector<std::string>		_server_blocks;
		int								_serv_num;
		servers_array					_servers;
};

#endif
