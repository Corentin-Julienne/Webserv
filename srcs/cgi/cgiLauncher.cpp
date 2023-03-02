/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiLauncher.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjulienn <cjulienn@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 20:48:16 by cjulienn          #+#    #+#             */
/*   Updated: 2023/03/02 15:24:15 by cjulienn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgiLauncher.hpp"

cgiLauncher::cgiLauncher()
{
	this->_initEnv();
	this->_convStrToSplit();
	this->_execCgi();
}

cgiLauncher::~cgiLauncher() 
{
	// freeing the char **_char_env
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

/* init a map storing all the env values, now a placeholder 
see : https://fr.wikipedia.org/wiki/Variables_d%27environnement_CGI for an exhaustive list of CGI env variables */
void	cgiLauncher::_initEnv(void)
{
	/* server related variables */
	this->_env["SERVER_SOFTWARE"] = "";
	this->_env["SERVER_NAME"] = "";
	this->_env["GATEWAY_INTERFACE"] = "";

	/* request related variables */
	this->_env["SERVER_PROTOCOL"] = "";
	this->_env["SERVER_PORT"] = "";
	this->_env["REQUEST_METHOD"] = "";
	this->_env["PATH_INFO"] = "";
	this->_env["PATH_TRANSLATED"] = "";
	this->_env["SCRIPT_NAME"] = "";
	this->_env["QUERY_STRING"] = "";
	this->_env["REMOTE_HOST"] = "";
	this->_env["REMOTE_ADDRESS"] = "";
	this->_env["AUTH_TYPE"] = "";
	this->_env["REMOTE_USER"] = "";
	this->_env["REMOTE_INDENT"] = "";
	this->_env["CONTENT_TYPE"] = "";
	this->_env["CONTENT_LENGTH"] = "";

	/* client related variables */
	this->_env["HTTP_ACCEPT"] = "";
	this->_env["HTTP_ACCEPT_LANGUAGE"] = "";
	this->_env["HTTP_USER_AGENT"] = "";
	this->_env["HTTP_COOKIE"] = "";
	this->_env["HTTP_REFERER"] = "";
}

/* create a char ** from the env to be able to use it with execve to trigger the CGI execution */
void	cgiLauncher::_convStrToSplit(void) // to test
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
		this->_char_env[i][env_val.size()] = NULL;
		i++;
	}
	this->_char_env[this->_env.size()] = NULL; // last line
}

/* serve a std::string called this->_output. Is the result of calling the CGI that will serve the content to 
the webserv, that needs to serve it to the client (i.e web browser) */
std::string	cgiLauncher::_execCgi(void)
{
	// std::string to serve
	std::string		output;
	// saving original fds for later
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
		close(fds[0]); 					// no need to read anything
		dup2(fds[1], STDOUT_FILENO);	// need to change stdout
		close(fds[1]);
		execve("name_of_the_file.cgi", NULL, this->_char_env); // add good params, name_of_the_file should be SCRIPT_NAME
		// case execve failure
		std::cerr << "execve failed and returned -1" << std::endl;
		return ("error 5xx"); // change this (using write ?)
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
		// read the output using read
		ssize_t		reader = 1;
		char		buffer[1024];
		
		while (reader > 0)
		{
			reader = read(STDIN_FILENO, buffer, 1023); // check this
			if (reader < 0)
			{
				std::cerr << "read syscall failure" << std::endl;
				exit(EXIT_FAILURE);
			}
			output += buffer;
		}
	}

	// reset STDIN and STDOUT
	dup2(originalStdin, STDIN_FILENO);
	close(originalStdin);
	dup2(originalStdout, STDOUT_FILENO);
	close(originalStdout);

	return (output);
}
