/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 08:10:37 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/11 11:12:59 by spider-ma        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

/* create a Parser object that will extract relevant inforamtion from the conf file 
and store it in a ServConf struct */
Parser::Parser(char *config_file)
{
	this->_conf_file.open(config_file, std::ios_base::in);
	if (!this->_conf_file.is_open())
		throw std::runtime_error("failure to open the conf file. Check that you provide the good path");
	this->_processFile();
	this->_serv_num = this->_server_blocks.size();	
	for (int i = 0; i < this->_serv_num; i++)
	{
		this->_servers.push_back(ServConf());
		this->_processBlock(this->_server_blocks[i], i);
	}
	if (!this->_isThereEnoughInfo())
		throw std::runtime_error("please provide conf file with a cgi, at least one location and one default_server maximum");
	// debug
	//this->displayParsing();
}

/* case no conf file is provided */
Parser::Parser(void)
{
	Location	loc;
	
	// add only one sever block
	this->_servers.push_back(ServConf());
	// populate this server with a port (8080), which is the default server, an index and a root
	this->_servers[0]._port = 8080;
	// add cgi 
	this->_servers[0]._cgi.push_back(".php");
	this->_servers[0]._cgi.push_back("srcs/cgi/php-cgi");
	// add a location block with value /
	this->_servers[0]._locs.push_back(loc);
	// population location with URL, and allow GET method
	this->_servers[0]._locs[0]._allowed_http_methods.push_back("GET");
	this->_servers[0]._locs[0]._url = "/";
	this->_servers[0]._locs[0]._root = "www/html";
	this->_servers[0]._locs[0]._index.push_back("index.html");
	// debug
	//this->displayDefaultParsing();
}

Parser::~Parser() {}

Parser::Parser(const Parser& original) : _conf_str(original._conf_str), _server_blocks(original._server_blocks),
_serv_num(original._serv_num), _servers(original._servers) {}

Parser&	Parser::operator=(const Parser &original)
{
	if (this != &original)
	{
		this->_conf_str = original._conf_str;
		this->_server_blocks = original._server_blocks;
		this->_serv_num = original._serv_num;
		this->_servers = original._servers;
	}
	return *this;
}

/* check whether the file is in valid format or not. Includes : 
=> file is not empty
=> there is at least a server part
=> */
void	Parser::_processFile(void)
{
	if (this->_conf_file.peek() == std::ifstream::traits_type::eof())
		throw std::runtime_error("configuration file is empty");
	this->_ifstreamToStr();
	this->_conf_file.close();
	if (!this->_isBlockSyntaxValid())
		throw std::runtime_error("syntax error : wrong number of brackets and/or invalid bracket order");
	this->_iterateThroughStr();
}

/* convert from ifstream format to std::string */
void	Parser::_ifstreamToStr(void)
{
	std::stringstream		buffer;

	buffer << this->_conf_file.rdbuf();
	this->_conf_str = buffer.str();
}

/* check till EOF that there is a matching number of { and } and that the first is { */
bool	Parser::_isBlockSyntaxValid(void)
{
	std::size_t		i = 0;
	int				num_open_brack = 0;
	int				num_close_brack = 0;
	bool			first_bracket = false;

	while (i < this->_conf_str.size())
	{
		if (!first_bracket && this->_conf_str[i] == '{')
			first_bracket = true;
		if (!first_bracket && this->_conf_str[i] == '}')
			return (false);
		if (this->_conf_str[i] == '{')
			num_open_brack++;
		if (this->_conf_str[i] == '}')
			num_close_brack++;
		i++;
	}
	return (num_open_brack > 0 && (num_open_brack == num_close_brack) ? true : false);
}

/* goal is to extract every server block within a vector and check if server conf is correct */
void	Parser::_iterateThroughStr(void)
{
	std::string			block;
	std::size_t			i = 0;
	
	while (i < this->_conf_str.size())
	{
		if (!std::isspace(this->_conf_str[i]) && this->_conf_str[i] != 's')
			throw std::runtime_error("only server blocks are allowed in the main scope");
		if (this->_conf_str[i] == 's')
		{
			if (!this->_conf_str.compare(i, 6, "server"))
			{
				i += 6;
				while (this->_conf_str[i] != '{')
				{
					if (!std::isspace(this->_conf_str[i]))
						throw std::runtime_error("syntax error on the server directive");
					i++;
				}
				i += this->_splitServerBlock(i) - 1;
			}
			else
				throw std::runtime_error("only server blocks are allowed in the main scope");
		}
		i++;
	}
}

/* extract a server block of type std::string without the server keywork and the brackets */
int	Parser::_splitServerBlock(int i)
{
	std::string 	block;
	int				size;

	block = this->_extractBracketsBlock(this->_conf_str.substr(i));	
	if (!this->_isServerBlockValid(block))
		throw std::runtime_error("invalid number of brackets in server block");
	size = block.size();
	block = block.substr(1, block.size() - 2); // remove {}
	if (!this->_areBracketsPopulated(block))
	 	throw std::runtime_error("empty set of brackets detected");
	this->_server_blocks.push_back(block);
	return (size);
}

/* extracts a set of brackets. Content should begin by char '{'
and will extract {<content_between_brackets>}. It returns also the brackets */
std::string	Parser::_extractBracketsBlock(std::string content)
{
	std::size_t			i = 1;
	std::size_t			open_brack = 1;
	std::size_t			close_brack = 0;
	
	while (i < content.size())
	{	
		if (content[i] == '{')
			open_brack++;
		if (content[i] == '}')
		{
			close_brack++;
			if (open_brack == close_brack)
				return (content.substr(0, i + 1));
		}
		i++;
	}
	throw std::runtime_error("unclose brackets on block");
}

/* return parsing infos */
const Parser::servers_array Parser::getServers(void) const
{
	return (this->_servers);
}

/* are brackets populated takes a substring content and check whether it is empty
used recursively when founding a nested set of brackets 
String content should be trimmed from brackets */
bool	Parser::_areBracketsPopulated(std::string content)
{
	std::size_t		i = 0;
	std::size_t		non_space_char = 0;
	std::string		subcontent;
	
	while (i < content.size())
	{
		if (!std::isspace(content[i]) && content[i] != '{')
			non_space_char++;
		if (content[i] == '{' && !non_space_char)
			return (false);
		else if (content[i] == '{' && non_space_char)
		{
			subcontent = this->_extractBracketsBlock(content.substr(i));
			subcontent = subcontent.substr(1, subcontent.size() - 2); // trimming brackets
			return this->_areBracketsPopulated(subcontent);
		}
		i++;
	}
	if (!non_space_char)
		return (false);
	return (true);
}

/* check whether there is the same number of {} in the string,
plus returns false if there is nothing between brackets */
bool	Parser::_isServerBlockValid(std::string substr)
{
	int				num_open_brack = 0;
	int				num_close_brack = 0;
	std::size_t		i = 0;

	while (i < substr.size())
	{
		if (substr[i] == '{')
			num_open_brack++;
		if (substr[i] == '}')
			num_close_brack++;
		i++;
	}
	return ((num_open_brack == num_close_brack) ? true : false && num_open_brack > 0);
}

/* Utils */

/* Takes the directive that finish by char delim. Returns an vector containing the arguments */
std::vector<std::string>	Parser::_cutArgs(std::string directive, char delim)
{
	std::vector<std::string>		args;
	int								i = 0;
	int								size_arg = 0;

	while (directive[i] != delim)
	{
		while (std::isspace(directive[i]))
			i++;
		while (!std::isspace(directive[i + size_arg]) && directive[i + size_arg] != delim)
			size_arg++;
		if (size_arg > 1 || (size_arg == 1 && directive[i + size_arg] != delim))
			args.push_back(directive.substr(i, size_arg));
		else
			break ;
		i += size_arg;
		size_arg = 0;
	}
	return (args);
}

/* if directive refers to a location, triggers the specialized function. Otherwise,
check if the directive format is valid (have at least two arguments, is bounded by ';').
Then, trigger relevant processing functions. Returns the length of chars used by the directive */
int	Parser::_dispatchInstructionProcessing(int type, std::string directive, int serv_idx, bool is_loc)
{
	std::size_t		args_num;
	std::size_t		dir_len;

	if (type == LOCATION)
		return (this->_processLocationBlock(directive, serv_idx));
	if (!this->_isDirectiveValid(directive))
		throw std::runtime_error("invalid directive syntax");
	args_num = this->_cutArgs(directive, ';').size();
	dir_len = directive.substr(0, directive.find(";")).size() + 1;

	switch (type)
	{
		case (LISTEN):
			this->_processListenDirective(directive, serv_idx, args_num, is_loc);
			break ;
		case (SERV_NAME):
			this->_processServerNameDirective(directive, serv_idx, args_num, is_loc);
			break ;
		case (ERR_PAGE):
			this->_processErrorPageDirective(directive, serv_idx, args_num, is_loc);
			break ;
		case (CLIENT_BODY_SIZE):
			this->_processBodySizeDirective(directive, serv_idx, args_num, is_loc);
			break ;
		case (ALLOW_HTTP_METHOD):
			this->_processAllowDirective(directive, serv_idx, args_num, is_loc);
			break ;
		case (ROOT):
			this->_processRootDirective(directive, serv_idx, args_num, is_loc);
			break ;
		case (AUTOINDEX):
			this->_processAutoindexDirective(directive, serv_idx, args_num, is_loc);
			break ;
		case (INDEX):
			this->_processIndexDirective(directive, serv_idx, args_num, is_loc);
			break ;
		case (RETURN):
			this->_processReturnDirective(directive, serv_idx, args_num, is_loc);
			break ;
		case (CGI):
			this->_processCGI(directive, serv_idx, args_num, is_loc);
			break ;
		default:
			throw std::runtime_error("instruction unknown");
			break ;
	}
	return (dir_len);
}

int	Parser::_rtnInstructionType(std::string directive)
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
	else if (!directive.compare("root"))
		return (ROOT);
	else if (!directive.compare("autoindex"))
		return (AUTOINDEX);
	else if (!directive.compare("index"))
		return (INDEX);
	else if (!directive.compare("return"))
		return (RETURN);
	else if (!directive.compare("cgi"))
		return (CGI);
	else
		return (BAD_INSTR);
}
