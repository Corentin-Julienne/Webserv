/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessBlocks.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 12:15:36 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/01 11:08:38 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

/* process the text of config for every block (location or server block). 
No server or location directive should be present present in the block, 
neither trailing whitespaces or initial { and } */
void	Parser::_processBlock(std::string block, int server_index, bool is_loc)
{
	std::size_t		i = 0;
	int				dir_type;
	
	while (i < block.size())
	{
		if (std::isalnum(block[i])) // if beginning of an instruction
		{
			dir_type= this->_rtnInstructionType(block.substr(i, block.substr(i).find(' '))); // test this			
			std::cout << "instruction = |" << block.substr(i, block.substr(i).find(' ')) << "|" << std::endl;
			if (dir_type == BAD_INSTR)
			{
				std::cout << "go there" << std::endl;
				std::cerr << "invalid instruction provided bro" << std::endl;
				exit(EXIT_FAILURE); // error, instruction does not exists
			}
			if (dir_type == LOCATION && is_loc == true)
			{
				std::cerr << "nested location feature not supported !!!" << std::endl;
				exit(EXIT_FAILURE);
			}
			else //if (block.substr(i).find(';') != std::string::npos) // add a verif condition
			{
				std::cout << "block sent to dispatcher = ||" << std::endl << block.substr(i) << "||" << std::endl << std::endl;
				i += this->_dispatchInstructionProcessing(dir_type, block.substr(i), server_index, is_loc);
				std::cout << "block received by dispatcher = |||"  << std::endl << block.substr(i) << "|||" << std::endl << std::endl;
			}
			// else 
			// 	break ;
		}
		i++;
	}
}

/* check if the syntax of the location block is valid or not, then extract the location block into
std::string block and process it after enforcing inheritance */
std::size_t	Parser::_processLocationBlock(std::string directive, int server_index)
{
	std::string						block;
	Location						loc;
	std::size_t						begin_idx;
	std::size_t						end_idx;
	
	if (!this->_isLocationBlockValid(directive))
	{
		std::cerr << "location block syntax is invalid" << std::endl;
		exit(EXIT_FAILURE); // handle error there
	}
	begin_idx = directive.find("{");
	end_idx	= directive.find("}");
	block = directive.substr(begin_idx + 1, end_idx - begin_idx - 1);	
	this->_servers[server_index]._locs.push_back(loc);
	this->_enforceInheritance(loc, server_index);	
	this->_processBlock(block, server_index, true);
	std::cout << "went after process location block" << std::endl;
	return (end_idx + 1);
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

/* Takes the directive that finish by char delim. Returns an vector containing the arguments */
std::vector<std::string>	Parser::_cutArgs(std::string directive, char delim) // to test
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

/* check whether the location block have two arguments (location and the path)
Checks also if there is one { and one } */
bool	Parser::_isLocationBlockValid(std::string block) // to test
{
	std::size_t			i = 0;
	std::size_t			start;

	while (std::isspace(block[i]) && i < block.size())
		i++;
	if (i == block.size() || block.substr(i).find("{") == std::string::npos 
	|| this->_cutArgs(block.substr(i), '{').size() != 2)
		return (false);
	start = block.substr(i).find("{");
	if (start == block.size())
		return (false);
	start++;
	if (block.substr(start).find("}") == std::string::npos)
		return (false);
	return (true);
	
	// std::vector<std::size_t>		nums;

	// nums.push_back(1); // boolean if valid or not
	// nums.push_back(0);

	// std::cout << "||" << block << "||" << std::endl;
	// while (std::isspace(block[nums[1]]) && nums[1] < block.size())
	// 	nums[1]++;
	// if (block.substr(nums[1]).find("{") == std::string::npos
	// || this->_cutArgs(block.substr(nums[1]), '{').size() != 2)
	// {
	// 	nums[0] = 0;
	// 	return (nums);
	// }
	// nums.push_back(nums[1]); // position of '{'
	// while (nums[1] < block.size() && block[nums[1]] != '}')
	// 	nums[1]++;
	// if (block[nums[1]] != '}')
	// 	nums[2] = 0;
	// return (nums);
}

/* check if directive is valid, returns also the lenght used and the number of arguments 
the vector val contains 3 std::size_t values : 
[0] = lenght of the directive
[1] = num of arguments
[2] = pseudo-boolean : 0 is valid, 1 is invalid */
// std::vector<std::size_t>	Parser::_isDirectiveValid(std::string directive) // seems quite ok
// {
// 	std::vector<std::size_t>		vals(3, 0);
// 	std::string						arg;
// 	int								last_char = 0;
 
// 	while (vals[0] < directive.size() && directive[vals[0]] != ';')
// 	{
// 		while (std::isspace(directive[vals[0]])) // trailing whitespaces
// 			vals[0]++;
// 		while (!std::isspace(directive[vals[0] + last_char]) && (directive[vals[0] + last_char]) != ';')
// 			last_char++;
// 		arg = directive.substr(vals[0], last_char);
// 		vals[1]++;
// 		vals[0] += arg.size();
// 	}
// 	if (vals[0] == directive.size()) // case reach EOF without finding a ';'
// 		vals[2] = 1;
// 	return (vals);
// }

/* check whether there is at least an argument in the */
bool	Parser::_isDirectiveValid(std::string directive)
{
	if (directive.find(";") == std::string::npos)
		return (false) ;
	if (this->_cutArgs(directive, ';').size() < 2)
		return (false);
	return (true);
}
