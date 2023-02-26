/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 08:10:37 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/26 14:57:44 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(char *config_file) // to test
{
	this->_openFile(config_file);
	this->_isFileValid();
	this->_serv_num = this->_server_blocks.size();
	for (std::size_t i = 0; i < this->_serv_num; i++)
	{
			this->_servers.push_back(ServConf()); // add a new Config to the linked list to be able to fulfill it
		this->_processBlock(this->_server_blocks[i], i);
	}
}

Parser::~Parser() {}

Parser::Parser(const Parser& original) : _conf_file(original._conf_file), _conf_str(original._conf_str),
_server_blocks(original._server_blocks), _serv_num(original._serv_num), _servers(original._servers) {}

Parser&	Parser::operator=(const Parser &original)
{
	if (this != &original)
	{
		this->_conf_file = original._conf_file;
		this->_conf_str = original._conf_str;
		this->_server_blocks = original._server_blocks;
		this->_serv_num = original._serv_num;
		this->_servers = original._servers;
	}
	return *this;
}




void	Parser::_openFile(char *config_file) // to test
{
	std::ifstream		conf_file;

	conf_file.open(config_file, std::ios_base::in);
	if (!conf_file.is_open())
	{
		std::cerr << "failure to open the conf file" << std::endl;
		exit(EXIT_FAILURE); // handle error there
	}
	this->_conf_file = conf_file;
}

/* check whether the file is in valid format or not. Includes : 
=> file is not empty
=> there is at least a server part
=> */
bool	Parser::_isFileValid(void) // to test
{
	// case file is empty
	if (this->_conf_file.peek() == std::ifstream::traits_type::eof())
	{
		std::cerr << "conf file is empty !!!" << std::endl;
		exit(EXIT_FAILURE); // case file is empty, handle error
	}
	this->_ifstreamToStr();
	this->_conf_file.close(); // handle possible errors there
	if (!this->_isBlockSyntaxValid()) // maybe not useful
	{
		std::cerr << "syntax error in the number of parenthesis" << std::endl;
		exit(EXIT_FAILURE);
	}
	this->_iterateThroughStr();
}

/* convert from ifstream format to std::string */
void	Parser::_ifstreamToStr(void) // to test
{
	std::stringstream		buffer;

	buffer << this->_conf_file.rdbuf();
	this->_conf_str = buffer.str();
}

/* check till EOF that there is a matching number of { and } and that the first is { */
bool	Parser::_isBlockSyntaxValid(void)
{
	int		i = 0;
	int		num_open_par = 0;
	int		num_close_par = 0;
	bool	first_bracket = false;

	while (this->_conf_str[i] < this->_conf_str.size())
	{
		if (!first_bracket && this->_conf_str[i] == '{')
			first_bracket = true;
		if (!first_bracket && this->_conf_str[i] == '}')
			return (false);
		if (this->_conf_str[i] == '{')
			num_open_par++;
		if (this->_conf_str[i] == '}')
			num_close_par++;
		i++;
	}
	return (num_open_par > 0 && (num_open_par == num_close_par) ? true : false);
}

/* goal is to extract every server block within a vector and check if server conf is correct */
void	Parser::_iterateThroughStr(void) // to test
{
	std::string			block;
	std::string 		auth_char(" \ns");
	int					i = 0;
	
	while (i < this->_conf_str.size())
	{
		if (auth_char.find(this->_conf_str[i]) == std::string::npos) // check whether char is not a ' ', '\n' or 's'
		{
			std::cerr << "wrong format detected" << std::endl;
			exit(EXIT_FAILURE); // handle error there
		}
		if (this->_conf_str[i] == 's') // if s is the beginning of the keyword server
		{
			if (this->_conf_str.compare(i, 6, "server") == 0) // check whether there is only whitespace before '{'
			{
				i += 6;
				while (this->_conf_str[i] != '{')
				{
					if (!std::isspace(this->_conf_str[i]))
					{
						std::cerr << "wrong format detected" << std::endl;
						exit(EXIT_FAILURE); // handle error there
					}
					i++;
				}
				i += this->_splitServerBlock(i);
			}
			else
			{
				std::cerr << "wrong format detected" << std::endl;
				exit(EXIT_FAILURE); // handle error	
			} 
		}
		i++;
	}
}

int	Parser::_splitServerBlock(int i) // to test
{
	std::size_t		start;
	std::string 	substr;
	int				size;
	
	start = this->_conf_str.find("server", i);
	if (start = std::string::npos) // case server block is the last
		substr = this->_conf_str.substr(i);
	else // case server block is not the last one
		substr = this->_conf_str.substr(i, start - i);
	if (!this->_isServerBlockValid(substr))
	{
		std::cerr << "wrong format detected" << std::endl;
		exit(EXIT_FAILURE);
		//handle error
	}
	size = substr.size();
	// need to trail whitespace and { and }
	while (std::isspace(substr[0])) // trim whitespace before
		substr = substr.substr(1, substr.size() - 1);	
	while (std::isspace(substr[substr.size() - 1])) // trim whitespaces after
		substr = substr.substr(0, substr.size() - 1);
	substr = substr.substr(1, substr.size() - 2); // remove {}
	// then push it back
	this->_server_blocks.push_back(substr); // should push back server block without 'server', whitesapce,
	// and opening and closing brackets
	return (size);
}

/* check whether there is the same number of {} in the string */
bool	Parser::_isServerBlockValid(std::string substr) // to test
{
	int		num_open_par = 0;
	int		num_close_par = 0;
	int		i = 0;

	while (i < substr.size())
	{
		if (substr[i] == '{')
			num_open_par++;
		if (substr[i] == '}')
			num_close_par++;
		i++;
	}	
	return ((num_open_par == num_close_par) ? true : false && num_open_par > 0);
}

int	Parser::_dispatchInstructionProcessing(int type, std::string directive, int serv_idx, bool is_loc = false) // to test
{
	std::vector<int>	size_and_args;
	
	if (type == LOCATION)
		return (this->_processLocationBlock(directive, serv_idx));

	size_and_args = this->_isDirectiveValid(directive);
	if (size_and_args[0] == -1)
	{
		std::cerr << "invalid directive format" << std::endl;
		exit(EXIT_FAILURE);
	}
	switch (type)
	{
		case (LISTEN):
			this->_processListenDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		case (SERV_NAME):
			this->_processServerNameDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		case (ERR_PAGE):
			this->_processErrorPageDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		case (CLIENT_BODY_SIZE):
			this->_processBodySizeDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		case (ALLOW_HTTP_METHOD):
			this->_processAllowDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		case (REWRITE):
			this->_processRewriteDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		case (ROOT):
			this->_processRootDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		case (AUTOINDEX):
			this->_processAutoindexDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		case (INDEX):
			this->_processIndexDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		case (CGI):
			this->_processCgiDirective(directive, serv_idx, size_and_args[1], is_loc);
			break ;
		default:
			std::cerr << "instruction unknown" << std::endl;
			exit(EXIT_FAILURE);
			break ;
	}
	return (size_and_args[0]);
}

int	Parser::_rtnInstructionType(std::string directive) // to test
{	
	if (!directive.compare("location"))
		return (LOCATION);
	else if (!directive.compare("listen"))
		return (LISTEN);
	else if (!directive.compare("server_name"))
		return (SERV_NAME);
	else if (!directive.compare("error_page"))
		return (ERR_PAGE);
	else if (!directive.compare("client_max_body_size"))
		return (CLIENT_BODY_SIZE);
	else if (!directive.compare("allow"))
		return (ALLOW_HTTP_METHOD);
	else if (!directive.compare("rewrite"))
		return (REWRITE);
	else if (!directive.compare("root"))
		return (ROOT);
	else if (!directive.compare("autoindex"))
		return (AUTOINDEX);
	else if (!directive.compare("index"))
		return (INDEX);
	else if (!directive.compare("cgi"))
		return (CGI);
	else
		return (BAD_INSTR);
}

/* DEBUGGING */

void	Parser::displayParsing(void)
{
	int		i = 0;
	
	std::cout << "displaying all server block in once in std::string format" << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << "[" << this->_conf_str << "]" << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	
	std::cout << "displaying parsing server by server" << std::endl << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	
	while (i < this->_servers.size())
	{
		std::cout << "displaying server " << (i + 1) << " :" << std::endl;	
		std::cout << "1) displaying server in std::string format" << std::endl;
		std::cout << this->_server_blocks[i] << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;

		std::cout << "2) displaying server in ServConf format" << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;
		std::cout << "port = " << this->_servers[i]._port << std::endl;
		std::cout << "ip address = " << this->_servers[i]._ip_address << std::endl;
		std::cout << "default_server = " << this->_servers[i]._default_server << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._error_pages.size(); j++)
		{
			std::cout << "set of directives number " << j << " : " <<  std::endl;
			for (std::size_t k = 0; k < this->_servers[i]._error_pages[j].size(); k++)
				std::cout << "error page arg " << k << " = " << this->_servers[i]._error_pages[j][k] << std::endl;
		}
		std::cout << "client_max_blody_size = " << this->_servers[i]._client_max_body_size << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._allowed_http_methods.size() ; j++)
			std::cout << "https methods num " << j << " = " << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._rewrite.size(); j++)
		{
			std::cout << "displaying rewrite directive num " << j << std::endl;
			for (std::size_t k = 0; this->_servers[i]._rewrite[j].size(); k++)
				std::cout << "arg num " << k << " = " << this->_servers[i]._rewrite[j][k] << std::endl;
		}
		std::cout << "displaying root = " << this->_servers[i]._root << std::endl;
		std::cout << "displaying autoindex = " << this->_servers[i]._autoindex << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._index.size(); j++)
			std::cout << "displaying index = " << this->_servers[i]._index[j] << std::endl;
		for (std::size_t j = 0; j < this->_servers[i]._cgi.size(); j++)
			std::cout << "displaying cgi = " << this->_servers[i]._cgi[j] << std::endl;
		
		std::cout << "---------------------------------------------------------" << std::endl << std::endl;

		std::cout << "3) displaying every location in Location format" << std::endl;
		
		for (std::size_t j = 0; j < this->_servers[i]._locs.size(); j++)
			this->displayLocation(this->_servers[i]._locs[j]);		
		
		std::cout << std::endl << std::endl;
		i++;
	}
	std::cout << "---------------------------------------------------------" << std::endl << std::endl;
	std::cout << "server display ended" << std::endl;
}

void	Parser::displayLocation(Location& loc)
{
	std::cout << "display location directive error_pages" << std::endl;
	for (std::size_t i = 0; loc._error_pages.size(); i++)
	{
		std::cout << "displaying set of directive num " << i << " :" << std::endl;
		for (std::size_t j = 0; j < loc._error_pages[i].size(); j++)
			std::cout << "error pages arg " << j << " = " << loc._error_pages[i][j] << std::endl;
	}
	std::cout << "display client_max_body_size = " << loc._client_max_body_size << std::endl;
	for (std::size_t i = 0; ; i++)
		std::cout << std::cout << "display allow directive num " << i << " = " << loc._allowed_http_methods[i] << std::endl;
	
	for (std::size_t i = 0; i < loc._rewrite.size(); i++)
	{
		std::cout << "display set of rewrite directives num " << i << std::endl;
		for (std::size_t j = 0; j < loc._rewrite[i].size(); j++)
			std::cout << "rewrite arg " << j << " = " << loc._rewrite[i][j] << std::endl;
	}
	std::cout << "display root = " << loc._root << std::endl;
	std::cout << "display autoindex = " << loc._autoindex << std::endl;
	for (std::size_t i = 0; i < loc._index.size(); i++)
		std::cout << "display index arg num " << i << " = " << loc._index[i] << std::endl;
	for (std::size_t i = 0; i < loc._cgi.size(); i++)
		std::cout << "display cgi arg num " << i << " = " << loc._index[i] << std::endl;
	std::cout << "display url = " << loc._url << std::endl;
}
