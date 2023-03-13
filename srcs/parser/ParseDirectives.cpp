/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseDirectives.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 12:10:30 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/05 21:43:41 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

/* ParseDirectives.cpp contains all the specialized functions that will store evry directive and 
check if they are compliant with the syntaxic rules accepted by the server */

bool	Parser::_isIpValid(std::string ip)
{
	std::string		delimiter = ".";
	std::string		token;
	std::size_t		pos = 0;
	bool			stop = false;

	if (!ip.compare("localhost"))
		return (true);
	while (!stop)
	{
		if ((pos = ip.find(delimiter)) == std::string::npos)
			stop = true;
		token = ip.substr(0, pos);
		if (token.size() > 3)
			return (false);
		for (std::size_t j = 0; j < token.size(); j++)
		{
			if (!std::isdigit(token[j]))
				return (false);
		}
		if (atoi(token.c_str()) > 255)
			return (false);
		ip.erase(0, pos + delimiter.length());
	}
	return (true);
}

void	Parser::_processListenDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string>		args;
	std::string						port_str;
	int								port_int;

	if (is_loc)
		throw std::runtime_error("listen directive is not accepted in a location block");
	if (arg_num < 2 || arg_num > 3)
		throw std::runtime_error("listen directive between one and two arguments");
	args = this->_cutArgs(directive, ';');
	if (args[1].find(':') != std::string::npos) // case there is a ip separated by : and then a port number
	{
		if (!this->_isIpValid(args[1].substr(0, args[1].find(":"))))
			throw std::runtime_error("invalid format of IpV4 address provided in .conf file");
		this->_servers[serv_idx]._ip_address = args[1].substr(0, args[1].find(":"));	
		port_str = args[1].substr(args[1].find(":") + 1);
	}
	else
		port_str = args[1];
	if (port_str.size() > 5)
		throw std::runtime_error("port number should be between 0 and 65535");
	for (std::size_t i = 0; i < port_str.size(); i++)
	{
		if (!std::isdigit(port_str[i]))
			throw std::runtime_error("port number should be between 0 ans 65535");
	}
	port_int = atoi(port_str.c_str());
	if (port_int > MAX_PORT)
		throw std::runtime_error("port number should be between 0 ans 65535");
	this->_servers[serv_idx]._port = port_int;
	if (arg_num == 3) // case there is a default_server directive
	{
		if (args[2].compare("default_server"))
			throw std::runtime_error("third argument is for default_server only");
		this->_servers[serv_idx]._default_server = true;
	}
}

void	Parser::_processServerNameDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{	
	std::vector<std::string>		args;
	
	if (is_loc)
		throw std::runtime_error("server_name directive is not accepted in a location block");
	if (arg_num < 2)
		throw std::runtime_error("server_name take at least one argument");
	args = this->_cutArgs(directive, ';');

	for (std::size_t i = 1; i < args.size(); i++)
	{
		if (args[i][0] == '"' && args[i][args[i].size() - 1] == '"')
			args[i] = args[i].substr(1, args[i].size() - 2);
	}

	// clear the vector is there is already a std:string in the same server block or location block  
	if (!this->_servers[serv_idx]._server_name.empty())
		this->_servers[serv_idx]._server_name.clear();

	for (std::size_t i = 1; i < args.size(); i++)
		this->_servers[serv_idx]._server_name.push_back(args[i]);	
}

void	Parser::_processErrorPageDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string>	args;
	std::vector<std::string>	new_err_dir;
	
	if (arg_num < 3)
		throw std::runtime_error("error_page directive accept at least two arguments");
	args = this->_cutArgs(directive, ';');
	for (std::size_t i = 1; i < args.size(); i++)
		new_err_dir.push_back(args[i]);
	if (is_loc)
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._error_pages.push_back(new_err_dir);
	else
		this->_servers[serv_idx]._error_pages.push_back(new_err_dir);	
}

void	Parser::_processBodySizeDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string>	args;
	int							multiplicator = 1;
	
	if (arg_num != 2)
		throw std::runtime_error("client_max_body_size takes only one argument");
	args = this->_cutArgs(directive, ';');
	for (std::size_t i = 0; i < args[1].size(); i++)
	{
		if (!std::isdigit(args[1][i]) && i != args[1].size() - 1)
			throw std::runtime_error("invalid syntax in directive client_max_body_size");
		else if (!std::isdigit(args[1][i]) && i == args[1].size() - 1)
		{
			if (args[1][args[1].size() - 1] == 'M' || args[1][args.size() - 1] == 'm')
			{
				multiplicator = 1000000;
				args[1] = args[1].substr(0, args[1].size() - 1);
			}
			else
				throw std::runtime_error("invalid syntax in directive client_max_body_size");
		}
	}

	int				body_size_int;
	long long int	body_size = std::strtol(args[1].c_str(), NULL, 10);

	if (body_size == LONG_MAX)
		throw std::runtime_error("client_max_body_size value is superior to max long");
	if (body_size > (std::numeric_limits<int>::max() / multiplicator))
		throw std::runtime_error("client_max_body_size value is superior to max int");
	body_size_int = static_cast<int>(body_size);
	body_size_int *= multiplicator;	

	if (is_loc)
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._client_max_body_size = body_size_int;
	else	
		this->_servers[serv_idx]._client_max_body_size = body_size_int;
}

void	Parser::_processAllowDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string> 	args;

	if (arg_num < 2)
		throw std::runtime_error("allow directive needs at least one argument");
	args = this->_cutArgs(directive, ';');

	// check if the directive as already been used, and clear it if it is the case
	if (is_loc &&
	!this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._allowed_http_methods.empty())
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._allowed_http_methods.clear();
	else if (!this->_servers[serv_idx]._allowed_http_methods.empty())
		this->_servers[serv_idx]._allowed_http_methods.clear();
	
	for (std::size_t i = 1; i < args.size(); i++)
	{	
		if (args[i].compare("GET") && args[i].compare("POST") && args[i].compare("DELETE"))
			throw std::runtime_error("wrong type of http method or method not implemented");
		if (is_loc)
			this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._allowed_http_methods.push_back(args[i]);
		else
			this->_servers[serv_idx]._allowed_http_methods.push_back(args[i]);
	}
}

void	Parser::_processRootDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string>	args;

	if (arg_num != 2)
		throw std::runtime_error("root directive takes one argument");
	args = this->_cutArgs(directive, ';');

	// clear if lready present in the same server block or location
	if (is_loc &&
	!this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._root.empty())
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._root.clear();
	else if (!this->_servers[serv_idx]._root.empty())
		this->_servers[serv_idx]._root.clear();
	
	if (is_loc)
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._root = args[1];
	else
		this->_servers[serv_idx]._root = args[1];
}

void	Parser::_processAutoindexDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	bool						switch_autoindex = false;
	std::vector<std::string>	args;
	
	if (arg_num != 2)
		throw std::runtime_error("autoindex should only accept one arg : on or off");
	args = this->_cutArgs(directive, ';');
	if (!args[1].compare("on"))
		switch_autoindex = true;
	if (is_loc)
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._autoindex = switch_autoindex;
	else
		this->_servers[serv_idx]._autoindex = switch_autoindex;
}

void	Parser::_processIndexDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string>	args;
	std::vector<std::string>	index_values;

	if (arg_num < 2)
		throw std::runtime_error("index directive take at least one argument");
	args = this->_cutArgs(directive, ';');
	for (std::size_t i = 1; i < args.size(); i++)
		index_values.push_back(args[i]);

	// clear if lready present in the same server block or location
	if (is_loc &&
	!this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._index.empty())
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._index.clear();
	else if (!this->_servers[serv_idx]._index.empty())
		this->_servers[serv_idx]._index.clear();
		
	if (is_loc)
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._index = index_values;
	else
		this->_servers[serv_idx]._index = index_values;
}

void	Parser::_processCgiDirective(std::string directive, int serv_idx, int arg_num, bool is_loc) // implement later
{
	std::vector<std::string>	args;
	std::vector<std::string>	cgi_values;
	
	if (arg_num < 2)
		throw std::runtime_error("cgi directive take a least one argument");
	args = this->_cutArgs(directive, ';');
	for (std::size_t i = 1; i < args.size(); i++)
		cgi_values.push_back(args[i]);
	if (is_loc)
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._cgi = cgi_values;
	else
		this->_servers[serv_idx]._cgi = cgi_values;
}
