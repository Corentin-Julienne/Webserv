/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiLauncher.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 20:48:16 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/07 11:39:30 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgiLauncher.hpp"

cgiLauncher::cgiLauncher(SocketInfos &infos, Location &loc, ServConf &serv) : _infos(infos), _loc(loc), _serv(serv)
{
	char			buffer[FILENAME_MAX];
	char			*success = getcwd(buffer, FILENAME_MAX);

	if (success)
		this->_cwd = success;
	else
		; // raise exception there

	/* setup env and convert it to char** format to fit execve requirements */
	this->_initEnv();
	/* debug */
	// this->_printInfos(); // debug
	// this->_printEnv(); // debug
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

/* init a map storing all the env values, in alphabetical order */
void	cgiLauncher::_initEnv()
{
	/* constant env variables */
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["REDIRECT_STATUS"] = "200";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_SOFTWARE"] = "WEBSERV/1.0";
	/* type of request and POST specific env variables */
	_env["REQUEST_METHOD"] = _infos.reqType; // GET, POST or DELETE
	if (_env["REQUEST_METHOD"] == "POST") // used with POST only
	{
		_env["CONTENT_TYPE"] = _infos.headers["Content-Type"];
		_env["CONTENT_LENGTH"] = this->_numToStr(_infos.body.size());
	}
	/* query string extraction */
	_env["QUERY_STRING"] = _infos.queryString; // everything after ? in the URI
	/* server IP and port */
	_env["REMOTE_ADDR"] = _serv._ip_address; // address of the client making the request
	_env["SERVER_PORT"] = this->_numToStr(_serv._port); // 
	/* domain name, if existing */
	if (!this->_serv._server_name.empty())
		_env["SERVER_NAME"] = this->_serv._server_name.back();
	else
		_env["SERVER_NAME"] = _serv._ip_address;
	/* script infos */
	_env["SCRIPT_NAME"] = this->_cwd + "/" + _infos.absoluteURIPath; // placeholder
	_env["SCRIPT_FILENAME"] = this->_cwd + "/" + _infos.absoluteURIPath; // placeholder
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
		if (_infos.reqType == "POST")
			dup2(fds[0], STDIN_FILENO);
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);	// need to change stdout
		close(fds[1]);

		char	**argv = this->_getArgs(this->_cwd + "/" + _loc._root + "/php/php-cgi"); // placeholder
			
		/* debug, checks for existence and chmod for the cgi script */
		if (access(argv[0],F_OK) == -1)
			std::cerr << "php-cgi not found" << std::endl;
		if (access(argv[0], X_OK) == -1)
			std::cerr << "php-cgi not executable" << std::endl;
		
		if (execve(argv[0], argv, this->_char_env) == -1) // segfault there with POST
			std::cerr << "execve failed and returned -1" << std::endl;
		exit(EXIT_FAILURE); // error to change
	}
	else // parent process
	{
		/* write body to fds[1] */
		if (_infos.reqType == "POST")
			write(fds[1], _infos.body.c_str(), _infos.body.size());
		close(fds[1]);
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
			reader = read(STDIN_FILENO, buffer, 50000);
			if (reader < 0)
			{
				std::cerr << "read syscall failure" << std::endl;
				exit(EXIT_FAILURE);
			}
			output += buffer;
		}
	}

	std::cout << "|" << output << "|" << std::endl; // debug

	/* reset STDIN and STDOUT */
	dup2(originalStdin, STDIN_FILENO);
	close(originalStdin);
	dup2(originalStdout, STDOUT_FILENO);
	close(originalStdout);

	/* post treatment of the output */
	output = this->_extractCGIHeader(output);	
	output = this->_formatOutput(output);
	return (output);
}

std::string	cgiLauncher::_formatOutput(std::string output)
{
	std::stringstream	content;

	content << "HTTP/1.1 200 OK\n" << this->_output_headers << "\n\n" << output;
	return (content.str());
}

std::string	cgiLauncher::_numToStr(int num)
{
	std::stringstream		ss;

	ss << num;
	return (ss.str());
}

void	cgiLauncher::_addHeadersToEnv(void)
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

std::string	cgiLauncher::_trimWhitespaces(std::string str)
{
	std::string		trimmed_str = str;
	
	while (trimmed_str.size() > 0 && std::isspace(trimmed_str.front()))
		trimmed_str = trimmed_str.substr(1);
	while (trimmed_str.size() > 0 && std::isspace(trimmed_str.back()))
		trimmed_str = trimmed_str.substr(0, trimmed_str.size() - 1);
	return (trimmed_str);
}

/* will suppress the first line added by the PHP-CGI erase, the headers and store them in 
a std::string to further use */
std::string	cgiLauncher::_extractCGIHeader(std::string output) // do later
{
	std::string		clear_output = "";
	std::string		out_headers;
	
	/* extracting body */
	if (output.find("\r\n\r\n") != std::string::npos)
		clear_output = output.substr(output.find("\r\n\r\n"));
	if (clear_output.size() > 4)
		clear_output.substr(4);
	
	/* extracting headers and store them */
	output = output.substr(output.find_first_of('\n') + 1);
	out_headers = output.substr(0, output.find("\r\n\r\n"));
	this->_output_headers = out_headers;

	return (this->_trimWhitespaces(clear_output));
}

void	cgiLauncher::_printEnv(void)
{
	std::map<std::string, std::string>::iterator it = this->_env.begin();

	std::cerr << "printing ENV map" << std::endl;
	while (it != this->_env.end())
	{
		std::cerr << it->first << " = " << it->second << std::endl;
		it++;
	}
}

void	cgiLauncher::_printInfos(void)
{
	std::cerr << "printing infos..." << std::endl;
	std::cerr << "printing headers" << std::endl;
	std::map<std::string, std::string>::iterator	it = _infos.headers.begin();
	while (it != _infos.headers.end())
	{
		std::cerr << "key = " << it->first << " = " << it->second << std::endl;
		it++;	
	}
	std::cerr << "printing body if existing : ";
	if (!_infos.body.empty())
		std::cerr << _infos.body << std::endl;
	std::cerr << "printing uri : " << _infos.uri << std::endl;
	std::cerr << "printing query string : " << _infos.queryString << std::endl;
	std::cerr << "printing real URI path : " << _infos.absoluteURIPath << std::endl;
	std::cerr << "printing type of request : " << _infos.reqType << std::endl;
	std::cerr << "printing path to location : " << _infos.locPath << std::endl;
	std::cerr << "" << std::endl;
	std::cerr << "-------------------------------" << std::endl;
}
