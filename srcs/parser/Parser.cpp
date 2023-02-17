/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 08:10:37 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/17 17:26:14 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(char *argv)
{
	this->_openFile(argv);
	this->_isFileValid();
}

Parser::~Parser()
{
	
}

void	Parser::_openFile(char *argv)
{
	std::ifstream		conf_file;

	conf_file.open(argv, std::ios_base::in);
	if (!conf_file.is_open())
	{
		std::cerr << "failure to open the conf file" << std::endl;
		// handle error there
	}
	this->_conf_file = conf_file;
}

/* check whether the file is in valid format or not. Includes : 
=> file is not empty
=> there is at least a server part
=> 
*/
bool	Parser::_isFileValid(void)
{
	// case file is empty
	if (this->_conf_file.peek() == std::ifstream::traits_type::eof())
	{
		// case file is empty, handle error
		std::cerr << "conf file is empty !!!" << std::endl;
		exit(EXIT_FAILURE);
	}
	this->_ifstreamToStr();
	this->_iterateThroughStr();
}

/* goal is to extract every server block within a vector and check if server conf is correct */
void	Parser::_iterateThroughStr(void) // to test
{
	std::string			block;
	std::string 		auth_char(" \ns");
	int					i;
	
	i = 0;
	while (i < this->_conf_str.size())
	{
		// check wether char is not a ' ', '\n' or 's'
		if (auth_char.find(this->_conf_str[i]) == std::string::npos)
		{
			// handle error there
			std::cerr << "wrong format detected" << std::endl;
		}
		// means potentially a server block
		if (this->_conf_str[i] == 's')
		{
			// if s is the beginning of the keyword server
			if (this->_conf_str.compare(i, 6, "server") == 0)
			{
				// check whether there is only whitespace before {
				i += 6;
				while (this->_conf_str[i] != '{')
				{
					if (this->_conf_str[i] != ' ') // isspace ?
					{
						std::cerr << "wrong format detected" << std::endl;
						// handle error there
					}
					i++;
				}
				i += this->_splitServerBlock(i);
			}
			else
			{
				std::cerr << "wrong format detected" << std::endl;
				// handle error
			} 
		}
		i++;
	}
}

int	Parser::_splitServerBlock(int i) // to test
{
	std::size_t		start;
	std::string 	substr;
	
	start = this->_conf_str.find("server", i);
	substr = this->_conf_str.substr(i, start - i);
	if (!this->_isServerBlockValid(substr))
	{
		std::cerr << "wrong format detected" << std::endl;
		//handle error
	}
	this->_server_blocks.push_back(substr);
	return (substr.size());
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
	return ((num_open_par == num_close_par) ? true : false);
}

/* convert from ifstream format to std::string */
void	Parser::_ifstreamToStr(void) // to test
{
	std::stringstream		buffer;

	buffer << this->_conf_file.rdbuf();
	this->_conf_str = buffer.str();
}
