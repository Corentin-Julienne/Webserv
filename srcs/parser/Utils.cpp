/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 18:13:09 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/09 13:23:06 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

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

/* ParseDirectives.cpp contains all the specialized functions that will store every directive and 
check if they are compliant with the syntaxic rules accepted by the server */
// bool	Parser::_isIpValid(std::string ip)
// {
// 	std::string		delimiter = ".";
// 	std::string		token;
// 	std::size_t		pos = 0;
// 	bool			stop = false;

// 	if (!ip.compare("localhost"))
// 		return (true);
// 	while (!stop)
// 	{
// 		if ((pos = ip.find(delimiter)) == std::string::npos)
// 			stop = true;
// 		token = ip.substr(0, pos);
// 		if (token.size() > 3)
// 			return (false);
// 		for (std::size_t j = 0; j < token.size(); j++)
// 		{
// 			if (!std::isdigit(token[j]))
// 				return (false);
// 		}
// 		if (atoi(token.c_str()) > 255)
// 			return (false);
// 		ip.erase(0, pos + delimiter.length());
// 	}
// 	return (true);
// }

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
	else
		return (BAD_INSTR);
}
