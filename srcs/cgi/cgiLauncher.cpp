/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiLauncher.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 20:48:16 by cjulienn          #+#    #+#             */
/*   Updated: 2023/04/11 11:25:29 by spider-ma        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgiLauncher.hpp"

cgiLauncher::cgiLauncher(SocketInfos &infos, ServConf &serv, std::string cgi_path) : _infos(infos), _serv(serv),
_scriptPath(cgi_path)
{
	char			buffer[FILENAME_MAX];
	char			*success = getcwd(buffer, FILENAME_MAX);

	this->_cwd = success;
	/* setup env and convert it to char** format to fit execve requirements */
	this->_initEnv();
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

const int&	cgiLauncher::getCode(void) const
{
	return this->_code;
}

const std::string&	cgiLauncher::getOutput(void) const
{
	return this->_output;
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
	_env["REMOTE_ADDR"] = "0.0.0.0"; // address of the client making the request
	_env["SERVER_PORT"] = this->_numToStr(_serv._port); // 
	/* domain name, if existing */
	if (!this->_serv._server_name.empty())
		_env["SERVER_NAME"] = this->_serv._server_name.back();
	else
		_env["SERVER_NAME"] = "0.0.0.0";
	/* script infos */
	//_env["SCRIPT_NAME"] = this->_cwd + "/" + _infos.absoluteURIPath;
	_env["SCRIPT_FILENAME"] = this->_cwd + "/" + _infos.absoluteURIPath;
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
int	cgiLauncher::exec(void)
{		
	std::string		output;
	/* saving original fds for later */
	int		originalStdin;
	int		fds[2];

	if ((originalStdin = dup(STDIN_FILENO)) == -1)
	{
		std::cerr << "problem with syscall dup" << std::endl;
		return (500);
	}

	if (pipe(fds) == -1)
	{
		std::cerr << "pipe syscall failure" << std::endl;
		return (500);
	}
	
	pid_t pid = fork();

	if (pid < 0)
	{
		std::cerr << "failure of the fork syscall" << std::endl;
		return 500;
	}
	else if (pid == 0) // child process
	{		
		if (_infos.reqType == "POST")
		{
			if (dup2(fds[0], STDIN_FILENO) == -1)
				return (500);
		}
		close(fds[0]);
		if (dup2(fds[1], STDOUT_FILENO) == -1)
			return (500);
		close(fds[1]);

		char	**argv = this->_getArgs(this->_cwd + "/" + this->_scriptPath);
			
		/* debug, checks for existence and chmod for the cgi script */
		if (access(argv[0],F_OK) == -1)
			std::cerr << "cgi not found" << std::endl;
		if (access(argv[0], X_OK) == -1)
			std::cerr << "cgi not executable" << std::endl;
		
		if (execve(argv[0], argv, this->_char_env) == -1)
			std::cerr << "execve failed and returned -1" << std::endl;
		exit(EXIT_FAILURE);
	}
	else // parent process
	{
		/* write body to fds[1] */
		if (_infos.reqType == "POST")
		{
			if (write(fds[1], _infos.body.data(), _infos.body.size()) == -1)
				return (500);
		}
		close(fds[1]);
		if (dup2(fds[0], STDIN_FILENO) == -1)
			return (500);		
		close(fds[0]);

		int status;
		
		if (waitpid(pid, &status, 0) == -1)
		{
			std::cerr << "waitpid syscall failure" << std::endl;
			return (500);
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
				return (500);
			}
			output += buffer;
		}
	}

	/* reset STDIN and STDOUT */
	if (dup2(originalStdin, STDIN_FILENO) == -1)
		return (500);
	close(originalStdin);

	/* post treatment of the output */
	output = this->_extractCGIHeader(output);
	output = this->_formatOutput(output);
	this->_output = output;
	return (200);
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
std::string	cgiLauncher::_extractCGIHeader(std::string output)
{
	std::string		clear_output = "";
	std::string		out_headers;
	
	/* extracting body */
	if (output.find("\r\n\r\n") != std::string::npos)
		clear_output = output.substr(output.find("\r\n\r\n"));
	if (clear_output.size() > 4)
		clear_output.substr(4);
	
	/* extracting headers and store them */
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
	std::cerr << "printing uri : " << _infos.uri << std::endl;
	std::cerr << "printing query string : " << _infos.queryString << std::endl;
	std::cerr << "printing real URI path : " << _infos.absoluteURIPath << std::endl;
	std::cerr << "printing type of request : " << _infos.reqType << std::endl;
	std::cerr << "printing path to location : " << _infos.locPath << std::endl;
	std::cerr << "" << std::endl;
	std::cerr << "-------------------------------" << std::endl;
}
