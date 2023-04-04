/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiLauncher.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 20:48:16 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/04 14:09:44 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgiLauncher.hpp"

void	cgiLauncher::_printEnv(void)
{
	std::map<std::string, std::string>::iterator it = this->_env.begin();

	std::cerr << "printing ENV map" << std::endl;
	while (it != this->_env.end())
	{
		std::cerr << "key |" << it->first << "| = |" << it->second << "|" << std::endl;
		it++;
	}
}

void	cgiLauncher::_printInfos(void)
{
	std::cerr << "printing infos" << std::endl;
	std::cerr << "--------------" << std::endl;
	
	std::cerr << "printing headers" << std::endl;
	std::map<std::string, std::string>::iterator	it = _infos.headers.begin();
	while (it != _infos.headers.end())
	{
		std::cerr << "key = " << it->first << " = " << it->second << std::endl;
		it++;	
	}
	std::cerr << "printing body if existing" << std::endl;
	if (!_infos.body.empty())
		std::cerr << _infos.body << std::endl;
	std::cerr << "printing uri : " << _infos.uri << std::endl;
	std::cerr << "printing query string : " << _infos.queryString << std::endl;
	std::cerr << "printing real URI path : " << _infos.absoluteURIPath << std::endl;
	std::cerr << "printing type of request : " << _infos.reqType << std::endl;
}

cgiLauncher::cgiLauncher(SocketInfos &infos, Location &loc, ServConf &serv) : _infos(infos), _loc(loc), _serv(serv)
{
	//this->_printInfos();// debug only
	char			buffer[FILENAME_MAX];
	char			*success = getcwd(buffer, FILENAME_MAX);

	if (success)
		this->_cwd = success;
	else
		; // raise exception there

	
	/* setup env and convert it to char** format to fit execve requirements */
	this->_initEnv();

	/* debug */
	//this->_printInfos();
	//this->_printEnv(); // debug


	this->_StrEnvToCStrArray();
}

cgiLauncher::~cgiLauncher() 
{
	/* freeing the char **_char_env */
	for (std::size_t i = 0; i < this->_env.size(); i++)
		delete [] this->_char_env[i];	
	delete [] this->_char_env;
}

cgiLauncher::cgiLauncher(const cgiLauncher& original) : _env(original._env), _char_env(original._char_env),
_output(original._output) {}

cgiLauncher&	cgiLauncher::operator=(const cgiLauncher& original)
{
	if (this != &original)
	{
		this->_env = original._env;
		this->_char_env = original._char_env;
		this->_output = original._output;
	}
	return *this;
}

/* init a map storing all the env values, in alphabetical order 
MANDATORY ENV PARAMS ARE :
=>	CONTENT_LENGTH OK
=>	CONTENT_TYPE OK
=>	GATEWAY_INTERFACE OK
=>	PATH_INFO OK
=>	QUERY_STRING OK
=>	REQUEST_METHOD OK
=>	REMOTE_ADDR OK
=>	SCRIPT_NAME
=>	SERVER_NAME OK
=>	SERVER_PORT OK
=>	SERVER_PROTOCOL OK
=>	SERVER_SOFTWARE OK */
void	cgiLauncher::_initEnv()
{
	/* constant env variables */
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["REDIRECT_STATUS"] = "200";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_SOFTWARE"] = "WEBSERV/1.0";
	/* type of request and POST specific env variables */
	_env["REQUEST_METHOD"] = _infos.reqType;
	if (_env["REQUEST_METHOD"] == "POST") // to test
	{
		_env["CONTENT_TYPE"] = _infos.headers["Content-Type"];
		_env["CONTENT_LENGTH"] = this->_numToStr(_infos.body.size());
	}
	/* providing PATH_INFO (i.e. full path) to the CGI executable */
	//_env["PATH_INFO"] = _cwd + _infos.absoluteURIPath + "/index.php"; // DEBUG
	//std::cout << _env["PATH_INFO"] << std::endl;
	/* query string extraction */
	_env["QUERY_STRING"] = _infos.queryString;
	/* server IP and port */
	_env["REMOTE_ADDR"] = _serv._ip_address; // ip
	_env["SERVER_PORT"] = this->_numToStr(_serv._port);
	/* domain name, if existing */
	if (!this->_serv._server_name.empty())
		_env["SERVER_NAME"] = this->_serv._server_name.back();
	else
		_env["SERVER_NAME"] = _serv._ip_address;
	/* script infos */
	_env["SCRIPT_NAME"] = this->_cwd + this->_loc._root + "/php/index.php"; // placeholder
	_env["SCRIPT_FILENAME"] = this->_cwd + this->_loc._root + "/php/index.php"; // placeholder
	/* client related variables */
	this->_addHeadersToEnv();
}

/* create a char ** from the env to be able to use it with execve to trigger the CGI execution */
void	cgiLauncher::_StrEnvToCStrArray(void)
{
	std::string										env_val;
	std::size_t										i = 0;
	
	this->_char_env = new char*[this->_env.size() + 1];
	for (std::map<std::string, std::string>::iterator iter = this->_env.begin(); iter != this->_env.end(); iter++)
	{
		env_val = iter->first + "=" + iter->second;
		this->_char_env[i] = new char[env_val.size() + 1];
		for (std::size_t j = 0; j < env_val.size(); j++)
			this->_char_env[i][j] = env_val.at(j);
		this->_char_env[i][env_val.size()] = '\0';
		i++;
	}
	this->_char_env[this->_env.size()] = NULL;
}

char	**cgiLauncher::_getArgs(std::string path)
{
	char	**argv;

	argv = new char*[2];
	argv[0] = new char[path.size() + 1];
	for (std::size_t iter = 0; iter < path.size(); iter++)
		argv[0][iter] = path[iter];
	argv[0][path.size()] = '\0';
	argv[1] = NULL;
	return (argv);
}

char	**cgiLauncher::_getArgs(std::string path, std::string target)
{
	char		**argv;

	argv = new char*[3];
	argv[0] = new char[path.size() + 1];
	for (std::size_t iter = 0; iter < path.size(); iter++)
		argv[0][iter] = path[iter];
	argv[0][path.size()] = '\0';
	argv[1] = new char[target.size() + 1];
	for (std::size_t iter = 0; iter < target.size(); iter++)
		argv[1][iter] = target[iter];
	argv[1][target.size()] = '\0';
	argv[2] = NULL;
	return (argv);	
}

/* serve a std::string called this->_output. Is the result of calling the CGI that will serve the content to 
the webserv, that needs to serve it to the client (i.e web browser) */
std::string	cgiLauncher::exec(void)
{	
	std::string		output;
	/* saving original fds for later */
	int		originalStdout = dup(STDIN_FILENO);
	int		originalStdin = dup(STDOUT_FILENO);
	
	int		fds[2];
	
	if (pipe(fds) == -1)
	{
		std::cerr << "pipe syscall failure" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	pid_t pid = fork();

	if (pid < 0)
	{
		std::cerr << "failure of the fork syscall" << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (pid == 0) // child process
	{
		//std::string	target = this->_cwd + "/www/html/php/index.php"; // placeholder, change this
		
		close(fds[0]); 					// no need to read anything
		dup2(fds[1], STDOUT_FILENO);	// need to change stdout
		close(fds[1]);
		
		char	**argv = this->_getArgs(this->_cwd + "/www/html/php/php-cgi"); // add good params
		
		std::cout << argv[0] << std::endl;
		if (access(argv[0],F_OK) == -1)
			std::cerr << "php-cgi not found" << std::endl;
		if (access(argv[0], X_OK) == -1)
			std::cerr << "php-cgi not executable" << std::endl;
		
		if (execve(argv[0], argv, this->_char_env) == -1)
			std::cerr << "execve failed and returned -1" << std::endl;
		exit(EXIT_FAILURE); // error to change
	}
	else // parent process
	{
		close(fds[1]);  // no need to write anything
		dup2(fds[0], STDIN_FILENO);
		close(fds[0]);
		if (waitpid(pid, NULL, 0) == -1) // wait for child process to finish
		{
			std::cerr << "waitpid syscall failure" << std::endl;
			exit(EXIT_FAILURE);	
		}
		/* read the output using read */
		ssize_t		reader = 1;
		char		buffer[BUFFER_SIZE + 1];
		
		while (reader > 0)
		{
			memset(buffer, 0, BUFFER_SIZE);
			reader = read(STDIN_FILENO, buffer, BUFFER_SIZE);
			if (reader < 0)
			{
				std::cerr << "read syscall failure" << std::endl;
				exit(EXIT_FAILURE);
			}
			output += buffer;
		}
	}

	/* reset STDIN and STDOUT */
	dup2(originalStdin, STDIN_FILENO);
	close(originalStdin);
	dup2(originalStdout, STDOUT_FILENO);
	close(originalStdout);

	// std::cout << "---------------OUTPUT---------------" << std::endl;
	// std::cout << output << std::endl;
	// std::cout << "------------------------------------" << std::endl;
	//exit(EXIT_FAILURE);
	return (this->_removeCGIHeader(output));
}

std::string	cgiLauncher::_numToStr(int num)
{
	std::stringstream		ss;

	ss << num;
	return (ss.str());
}

void	cgiLauncher::_addHeadersToEnv(void) // to test
{
	std::string										header_key;
	std::string										header_val;
	std::string										prefix = "HTTP_";
	std::map<std::string, std::string>::iterator	it = _infos.headers.begin();

	while (it != _infos.headers.end())
	{
		/* modifying key */
		header_key = prefix + it->first;
		for (std::size_t i = 0; i < header_key.size(); i++)
		{
			header_key[i] = toupper(header_key[i]);
			if (header_key[i] == '-')
				header_key[i] = '_';
		}
		header_val = it->second;
		/* inserting to env */
		this->_env.insert(std::pair<std::string, std::string>(header_key, header_val));
		it++;
	}
}

std::string	cgiLauncher::_removeCGIHeader(std::string output)
{
	std::string		clear_output = "";
	std::size_t		pos;
	bool			reach_empty_line = false;
	std::string		token;
	std::size_t		spaces;

	while (!output.empty())
	{
		pos = output.find('\n');
		token = output.substr(0, pos + 1);
		if (output.find('\n') == std::string::npos)
			output.clear();
		else
			output.erase(0, pos + 1);
		
		spaces = 0;
		for (std::size_t i = 0; i < token.size(); i++)
		{
			if (std::isspace(token[i]))
				spaces++;
		}
		if (spaces == token.size())
		{
			reach_empty_line = true;
			continue ;
		}
		if (reach_empty_line == true)
			clear_output += token;
	}
	return (clear_output);
}
