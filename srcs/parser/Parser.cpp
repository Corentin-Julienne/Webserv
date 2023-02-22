/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 08:10:37 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/22 17:37:44 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(char *argv) // to test
{
	this->_openFile(argv);
	this->_isFileValid();
	this->_serv_num = this->_server_blocks.size();
	for (std::size_t i = 0; i < this->_serv_num; i++)
	{
			this->_servers.push_back(ServConf()); // add a new Config to the linked list to be able to fulfill it
		this->_processServerBlock(this->_server_blocks[i], i);
	}
}

Parser::~Parser() {}

void	Parser::_openFile(char *argv) // to test
{
	std::ifstream		conf_file;

	conf_file.open(argv, std::ios_base::in);
	if (!conf_file.is_open())
	{
		std::cerr << "failure to open the conf file" << std::endl;
		exit(EXIT_FAILURE); // handle error there
	}
	this->_conf_file = conf_file;
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
	if () // case server block is the last
		;
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

/* process the text of config for every server block. 
No server directive is present in the block, neither trailing whitespaces or initial { and } */
void	Parser::_processServerBlock(std::string block, int server_index)
{
	int				i = 0;
	int				dir_type;
	std::string		directive;

	while (i < block.size())
	{
		if (!std::isspace(block[i]) && !std::isalnum(block[i])) // check if invalid char
		{
			std::cerr << "invalid char during block iteration" << std::endl;
			exit(EXIT_FAILURE); // handle error there
		}
		if (std::isalnum(block[i])) // if beggining of an instructions
		{
			dir_type= this->_rtnInstructionType(block.substr(i, block.substr(i).find(' '))); // test this
			if (dir_type == BAD_INSTR)
			{
				std::cerr << "invalid instruction provided" << std::endl;
				exit(EXIT_FAILURE); // error, instruction does not exists
			}
			else
				i += this->_dispatchInstructionProcessing(dir_type, block.substr(i), server_index);
		}
		i++;
	}
}

int	Parser::_processLocationBlock(std::string directive, int server_index)
{
	if (!this->_isLocationBlockValid(directive))
	{
		std::cerr << "location block syntax is invalid" << std::endl;
		exit(EXIT_FAILURE); // handle error there
	}
	// continue there, extract every block
	
}

/* check whether the location block is valid syntaxically have equal number of { and }
should be equal to 2 unless you have another nested location block */
int	Parser::_isLocationBlockValid(std::string block) // to test
{
	int		i = 8; // size of the std::string "location"
	int		errors = 0;
	int		open_brack = 0;
	int		close_brack = 0;

	while (std::isspace(block[i]) && i < block.size()) // trailing whitespace
		i++;
	if (block[i] != '{') // check whether the block start by '{'
		return (1);
	else
		open_brack++;
	if (i < block.size() - 1)
		i++;
	while (i < block.size() && block[i] != '}')
	{	
		if (block[i] == 'l' && !block.substr(i, 8).compare("location")) // case another nested location block
			errors += this->_isLocationBlockValid(block.substr(i)); 	
		i++;
	}
	if (i < block.size() && block[i] == '}')
		close_brack++;
	if (close_brack != open_brack || open_brack != 1)
		errors++;
	return (errors);
}

int	Parser::_processListenDirective(std::string directive, int server_index)
{
	std::string		instr;

	instr = directive.substr(0, directive.find(';')); // check that
	// implement

	return (instr.size());
}

int	Parser::_dispatchInstructionProcessing(int type, std::string directive, int server_index) // to test
{
	switch (type)
	{
		case (LOCATION):
			return (this->_processLocationBlock(directive, server_index));
			break ;
		case (LISTEN):
			return (this->_processListenDirective(directive, server_index));
			break ;
		case (SERV_NAME):
			return ();
			break ;
		case (ERR_PAGE):
			return ();
			break ;
		case (CLIENT_BODY_SIZE):
			return ();
			break ;
		case (ALLOW_HTTP_METHOD):
			return ();
			break ;
		case (REWRITE):
			return ();
			break ;
		case (ROOT):
			return ();
			break ;
		case (AUTOINDEX):
			return ();
			break ;
		case (INDEX):
			return ();
			break ;
		case (CGI):
			return ();
			break ;
		default:
			std::cerr << "instruction unknown" << std::endl;
			break;
	}
	return (-1);
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
