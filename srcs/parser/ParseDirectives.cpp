/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseDirectives.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpeharpr <mpeharpr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 12:10:30 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/10 19:08:03 by mpeharpr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

void	Parser::_processListenDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string>		args;
	std::string						port_str;
	int								port_int;

	if (is_loc)
		throw std::runtime_error("listen directive is not accepted in a location block");
	if (arg_num < 2 || arg_num > 3)
		throw std::runtime_error("listen directive takes one or two arguments");
	args = this->_cutArgs(directive, ';');
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
			throw std::runtime_error("second argument is for default_server only");
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

	/* remove quotes if double quotes are povided */
	for (std::size_t i = 1; i < args.size(); i++)
	{
		if (args[i][0] == '"' && args[i][args[i].size() - 1] == '"')
			args[i] = args[i].substr(1, args[i].size() - 2);
	}

	/* clear the vector is there is already a std:string in the same server block or location block */  
	if (!this->_servers[serv_idx]._server_name.empty())
		this->_servers[serv_idx]._server_name.clear();

	for (std::size_t i = 1; i < args.size(); i++)
		this->_servers[serv_idx]._server_name.push_back(args[i]);
}

/* add a set of new error pages. Add a vector of string, which contains : 
=> error codes (first argument) 
=> html page to display */
void	Parser::_processErrorPageDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string>	args;
	std::vector<std::string>	new_err_dir;
	std::string					code;
	std::string					html_page;
	
	if (arg_num != 3)
		throw std::runtime_error("error_page directive accepts two arguments");
	args = this->_cutArgs(directive, ';');
	/* check that code is ok */
	code = args[1];
	if (code.size() != 3 || !std::isdigit(code[0]) || !std::isdigit(code[1]) || !std::isdigit(code[2]))
		throw std::runtime_error("error_page : not a valid http error code");
	
	/* check that last argument is an html page */
	html_page = args[2];
	if (html_page.size() > 5 && html_page.substr(html_page.size() - 5).compare(".html"))
		throw std::runtime_error("error_page directive last argument must be an html page");
	/* push argument to vector and add it to location or servconf */
	new_err_dir.push_back(code);
	new_err_dir.push_back(html_page);
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

	/* check if the directive as already been used, and clear it if it is the case */
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
		throw std::runtime_error("index directive takes at least one argument");
	args = this->_cutArgs(directive, ';');
	for (std::size_t i = 1; i < args.size(); i++)
		index_values.push_back(args[i]);

	// clear if already present in the same server block or location
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

void	Parser::_processReturnDirective(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string>	args;
	std::pair<int, std::string>	rtn_dir;
	std::string					code;
	std::string					url;
	int							code_int;

	if (arg_num != 3)
		throw std::runtime_error("return directive takes two arguments : a return code and a full url");
	args = this->_cutArgs(directive, ';');
	/* assess validity of redirection code */
	code = args[1];
	if (code.size() != 3 && code.compare("301"))
		throw std::runtime_error("error code must be 301");
	/* assess length of the scheme of the url */
	url = args[2];
	if (url.size() > 2048)
		throw std::runtime_error("url should be one of a location");
	/* store the code */
	for (std::size_t i = 0; i < code.size(); i++)
	{
		if (!std::isdigit(code[i]))
			throw std::runtime_error("return code number is invalid");
	}
	code_int = atoi(code.c_str());
	if (code_int < 300 || code_int > 399)
		throw std::runtime_error("return code number must be a 3XX code");
	/* store into servconf or location */
	rtn_dir.first = code_int;
	rtn_dir.second = url;
	if (is_loc)
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._return = rtn_dir;
	else
		this->_servers[serv_idx]._return = rtn_dir;
}

void	Parser::_processCGI(std::string directive, int serv_idx, int arg_num, bool is_loc)
{
	std::vector<std::string>	args;
	std::vector<std::string>	cgi_infos;
	std::string					extension;
	std::string					cgi_path;
	
	if (arg_num != 3)
		throw std::runtime_error("cgi directive takes two arguments : extension, path/executable_name");
	args = this->_cutArgs(directive, ';');

	/* check that extension is php or bla (for testing) */
	extension = args[1];
	if (extension.size() != 4 || (extension.compare(".php") && extension.compare(".bla")))
		throw std::runtime_error("extension have to be php (or bla for testing only)");
	/* check that path exists */
	
	char			buffer[FILENAME_MAX];
	char			*success = getcwd(buffer, FILENAME_MAX);
	std::string		curr_wd;
	std::string		full_path;
	std::string		cgi_name;

	if (success)
		curr_wd = success;
	else
		throw std::runtime_error("problem with getcwd syscall");

	cgi_path = args[2];
	if (cgi_path.find_last_of('/') == std::string::npos || cgi_path.substr(cgi_path.find_last_of('/')).size() < 1)
		throw std::runtime_error("wrong syntax of the path of the cgi script");
	if (cgi_path.substr(cgi_path.find_last_of('/') + 1).compare("php-cgi") && 
	cgi_path.substr(cgi_path.find_last_of('/') + 1).compare("cgi_tester"))
		throw std::runtime_error("cgi script not accepted. Should be php-cgi or cgi_tester");

	full_path = curr_wd + "/" + cgi_path;

	/* debug, checks for existence and chmod for the cgi script */
	if (access(full_path.c_str(), F_OK) == -1)
		throw std::runtime_error("cgi script not found");
	if (access(full_path.c_str(), X_OK) == -1)
		throw std::runtime_error("cgi script not executable");
	/* store the cgi informations */
	cgi_infos.push_back(extension);
	cgi_infos.push_back(cgi_path);
	/* store into servconf or location */
	if (is_loc)
		this->_servers[serv_idx]._locs[this->_servers[serv_idx]._locs.size() - 1]._cgi = cgi_infos;
	else
		this->_servers[serv_idx]._cgi = cgi_infos;
}
