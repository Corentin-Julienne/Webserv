/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 08:10:40 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/22 14:48:59 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServConf.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <cctype>

#define BAD_INSTR			0
#define LOCATION			1
#define LISTEN				2
#define SERV_NAME			3
#define ERR_PAGE			4
#define CLIENT_BODY_SIZE	5
#define ALLOW_HTTP_METHOD	6
#define REWRITE				7
#define ROOT				8
#define AUTOINDEX			9
#define INDEX				10
#define CGI					11

class Parser
{
	public:
	
		Parser(char *argv);
		~Parser();

	private:
	
		// ops on conf file
		void	_openFile(char *argv);
		bool	_isFileValid(void);
		void	_ifstreamToStr(void);
		// separating std::string on different server blocks
		void	_iterateThroughStr(void);
		int		_splitServerBlock(int i);
		bool	_isBlockSyntaxValid(void);
		bool	_isServerBlockValid(std::string substr);
		// process instructions
		void	_processServerBlock(std::string block, int server_index);
		int		_rtnInstructionType(std::string directive);
		int		_dispatchInstructionProcessing(int type, std::string directive, int server_index);
		int		_processInstruction(std::string directive);
		// process locations
		int		_processLocationBlock(std::string directive, int server_index);
		int		_isLocationBlockValid(std::string block);
		// process various instructions
		int		_processListenDirective(std::string directive, int server_index);
		
		std::ifstream				_conf_file;
		std::string					_conf_str;
		std::vector<std::string>	_server_blocks;
		int							_serv_num;
		std::vector<ServConf>		_servers;
};
