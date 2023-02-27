/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessBlocks.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 12:15:36 by cjulienn          #+#    #+#             */
/*   Updated: 2023/02/27 19:40:19 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

/* process the text of config for every block (location or server block). 
No server or location directive is present in the block, 
neither trailing whitespaces or initial { and } */
void	Parser::_processBlock(std::string block, int server_index, bool is_loc)
{
	std::size_t		i = 0;
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
			//std::cout << "item : |" << block.substr(i, block.substr(i).find(' ')) << "|" << std::endl;
			dir_type= this->_rtnInstructionType(block.substr(i, block.substr(i).find(' '))); // test this
			if (dir_type == BAD_INSTR)
			{
				std::cerr << "invalid instruction provided" << std::endl;
				exit(EXIT_FAILURE); // error, instruction does not exists
			}
			if (dir_type == LOCATION && is_loc == true)
			{
				std::cerr << "nested location feature not supported !!!" << std::endl;
				exit(EXIT_FAILURE);
			}
			else
				i += this->_dispatchInstructionProcessing(dir_type, block.substr(i), server_index, is_loc);
		}
		i++;
	}
}

/* check if the syntax of the location block is valid or not, then extract the location block into
std::string block and process it after enforcing inheritance */
std::size_t	Parser::_processLocationBlock(std::string directive, int server_index)
{
	std::vector<std::size_t>		nums;
	std::string						block;
	Location						loc;
	
	nums = this->_isLocationBlockValid(directive);
	if (nums[0] == 0)
	{
		std::cerr << "location block syntax is invalid" << std::endl;
		exit(EXIT_FAILURE); // handle error there
	}
	block = directive.substr(nums[2] + 1, nums[1] - 1);	
	this->_servers[server_index]._locs.push_back(loc);
	this->_enforceInheritance(loc, server_index);	
	this->_processBlock(block, server_index, true);
	return (nums[1]);
}

/* Copy the values of a server block to the Location struct loc, to enforce inheritance principle */
void	Parser::_enforceInheritance(Location& loc, int server_index) // to test
{
	loc._error_pages = this->_servers[server_index]._error_pages;
	loc._client_max_body_size = this->_servers[server_index]._client_max_body_size;
	loc._allowed_http_methods = this->_servers[server_index]._allowed_http_methods;
	loc._rewrite = this->_servers[server_index]._rewrite;
	loc._root = this->_servers[server_index]._root;
	loc._autoindex = this->_servers[server_index]._autoindex;
	loc._index = this->_servers[server_index]._index;
	loc._cgi = this->_servers[server_index]._cgi;
}

/* Takes the directive that finish by ';'. Returns an vector containing the arguments */
std::vector<std::string>	Parser::_cutArgs(std::string directive) // to test
{
	std::vector<std::string>		args;
	int								i = 0;
	int								size_arg = 0;

	while (directive[i] != ';')
	{
		while (std::isspace(directive[i]))
			i++;
		while (!std::isspace(directive[i + size_arg]) && directive[i + size_arg] != ';')
			size_arg++;
		if (size_arg > 0)
			args.push_back(directive.substr(i, size_arg));
		i += size_arg;
	}
	return (args);
}

/* check whether the location block is valid syntaxically have one { and one }
does not support nested location block like nginx !!!
return 0 if not the case, length of the location block otherwise */
std::vector<std::size_t>	Parser::_isLocationBlockValid(std::string block) // to test
{
	std::vector<std::size_t>		nums;

	nums.push_back(1); // boolean if valid or not
	nums.push_back(8); // iterator starting after "location" substring

	while (std::isspace(block[nums[1]]) && nums[1] < block.size())
		nums[1]++;
	if (block[nums[1]] != '{')
	{
		nums[0] = 0;
		return (nums);
	}
	else
		nums.push_back(nums[1]); // position of '{'
	while (nums[1] < block.size() && block[nums[1]] != '}')
		nums[1]++;
	if (block[nums[1]] != '}')
		nums[2] = 0;
	return (nums);
}

/* check if directive is valid, returns also the lenght used and the number of arguments 
the vector val contains 3 std::size_t values : 
[0] = lenght of the directive
[1] = num of arguments
[2] = pseudo-boolean : 0 is valid, 1 is invalid */
std::vector<std::size_t>	Parser::_isDirectiveValid(std::string directive) // seems quite ok
{
	std::vector<std::size_t>		vals(3, 0);
	std::string						arg;
	int								last_char = 0;
 
	while (vals[0] < directive.size() && directive[vals[0]] != ';')
	{
		while (std::isspace(directive[vals[0]])) // trailing whitespaces
			vals[0]++;
		while (!std::isspace(directive[vals[0] + last_char]) && (directive[vals[0] + last_char]) != ';')
			last_char++;
		arg = directive.substr(vals[0], last_char);
		vals[1]++;
		vals[0] += arg.size();
	}
	if (vals[0] == directive.size()) // case reach EOF without finding a ';'
		vals[2] = 1;
	return (vals);
}
