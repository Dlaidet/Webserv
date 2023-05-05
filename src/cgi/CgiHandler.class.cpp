/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.class.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amorel <amorel@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 16:13:25 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/04 12:07:51 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi/CgiHandler.class.hpp"

CgiHandler::CgiHandler()
{

}

/*
*  @brief   Default constructor.
*			Get the needed data to set the execution.
*  @param   RequestHandler, a request handler object
*  @return  void
*/
CgiHandler::CgiHandler(RequestHandler const & rh)
{
	// Declare a char array to store the current working directory and retrieve it using getcwd()
	char currentDir[1024];

	getcwd(currentDir, sizeof(currentDir));
	// Get the root directory from the RequestHandler object
	std::string root = rh.getRoot();
	// If the root directory is not empty and starts with a period, remove the period
	if (!root.empty() && root[0] == '.')
		root.erase(0, 1);
	// Construct the path to the CGI script by concatenating the current directory, root directory, and request URI
	this->_script = currentDir + root + rh.getRequestURI();
	// Get the interpreter path from the RequestHandler object
	this->_cgi_interpreter = rh.getCgiInterpreter();
	// Get the query string from the RequestHandler object as a map of key-value pairs
	std::map<std::string, std::string> tmp = rh.getQueryString();
	// Convert the query string map to a string and store it in the _query_string member variable
	this->_query_string = recomposeQueryString(tmp);
	// Get the HTTP method from the RequestHandler object
	this->_method = rh.getMethod();
	this->_status = 0;
	// Set environment variables required for the CGI script to execute
	this->setEnv();
}

/*
*  @brief   build a std::string from std::map<std::string, std::string>
*  @param   void
*  @return  std::string
*/
std::string	CgiHandler::recomposeQueryString(std::map<std::string, std::string> &query)
{
	std::ostringstream oss;

	for (std::map<std::string, std::string>::iterator it = query.begin(); it != query.end(); ++it)
	{
		if (it != query.begin())
			oss << "&";
		oss << it->first << "=" << it->second;
	}
	return (oss.str());
}

/*
*  @brief   Destructor.
*			Destroy all member objects
*  @param   void
*  @return  void
*/
CgiHandler::~CgiHandler() 
{

}

/*
*  @brief   checkCgi from non blocking client.
*		   check if the cgi is completed and read the result of the process.
*  @param   void
*  @return  void
*/
void CgiHandler::checkCgi()
{
	int	 wstatus;
	int	 ret;

	this->_status = 1;
	ret = waitpid(_pid, &wstatus, WNOHANG);
	if (ret < 0)
		this->_is_cgi_running = false;
	if (ret > 0)
	{
		if (WIFEXITED(wstatus))
		{
			WEXITSTATUS(wstatus);
			if (wstatus == 0)
			{
				char	buffer[65536];
				this->_status = 0;
				// sets the read end of pipeOut to non-blocking mode
				int flags = fcntl(this->_cgi_fd_out, F_GETFL, 0);
				fcntl(this->_cgi_fd_out, F_SETFL, flags | O_NONBLOCK);
				// Reading data from the output pipe and appending it to the body string
				memset(buffer, 0, 65536);
				ret = read(this->_cgi_fd_out, buffer, 65536 - 1);
				if (ret < 0)
					this->_status = 1;
				this->_body = buffer;
				this->_is_cgi_running = false;
                close(this->_cgi_fd_out);
			}
		}
        if (wstatus != 0)
            close(this->_cgi_fd_out);
	}
}

/*
*  @brief   return fd in
*  @param   void
*  @return  int
*/
int	CgiHandler::getFdIn() const
{
	return _cgi_fd_in;
}

/*
*  @brief   return fd out
*  @param   void
*  @return  int
*/
int	CgiHandler::getFdOut() const
{
	return _cgi_fd_out;
}

/*
*  @brief   return cgi status
*  @param   void
*  @return  bool
*/
bool	CgiHandler::isActive() const
{
	return _is_cgi_running;
}

/*
*  @brief   Set the environment variables needed for execve
*  @param   void
*  @return  void
*/
void		CgiHandler::setEnv(void)
{
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "FoxServ/1.0";
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SCRIPT_NAME"] = this->_script;
	this->_env["SCRIPT_FILENAME"] = this->_script;
	this->_env["REQUEST_METHOD"] = this->_method;
	this->_env["QUERY_STRING"] = this->_query_string;
}

/*
*  @brief   get the environment as a char * for execve
*  @param   void
*  @return  char ** env
*/
char **CgiHandler::getEnvChar(void)
{
	// allocate memory for the char* array, add 1 for NULL termination
	char	**env = new char*[this->_env.size() + 1];
	int	j = 0;

	// loop through the map of environment variables
	for (std::map<std::string, std::string>::const_iterator i = this->_env.begin(); i != this->_env.end(); i++)
	{
		// concatenate key and value with '=' and store in a string
		std::string	element = i->first + "=" + i->second;
		// allocate memory for the char* for this environment variable
		env[j] = new char[element.size() + 1];
		// copy the string to the char* array
		env[j] = strcpy(env[j], (const char*)element.c_str());
		j++;
	}
	// add NULL termination to the end of the array
	env[j] = NULL;
	// return the char* array
	return env;
}

/*
*  @brief   Used to exit a cgi if timeout (infinite loop case)
*  @param   void
*  @return  void
*/
void CgiHandler::alarm_handler(int signal_number)
{
	(void) signal_number;
	exit(1);
}

/*
*  @brief   This function executes a CGI script and returns the output generated by the script
*  @param   void
*  @return  std::string body
*/
void CgiHandler::executeCgi()
{
	int pipeIn[2], pipeOut[2];
	// Creating two pipes for input and output
	pipe(pipeIn);
	pipe(pipeOut);
	pid_t pid;

	this->_is_cgi_running = true;

	// Getting environment variables for the CGI script
	char		**env = getEnvChar();

	// Creating a child process using fork
	pid = fork();
	if (pid == -1)
		this->_status = 1;
	else if (!pid)
	{
		// In child process:
		// Creating arguments for execve system call
		char *const args[] = { const_cast<char*>(this->_cgi_interpreter.c_str()),
							   const_cast<char*>(this->_script.c_str()), NULL };
		// Redirecting standard input and output to the pipes
		close(pipeIn[1]);
		dup2(pipeIn[0], STDIN_FILENO);
		close(pipeIn[0]);
		close(pipeOut[0]);
		dup2(pipeOut[1], STDOUT_FILENO);
		close(pipeOut[1]);
		// Set an alarm signal to avoid infinite loops
		signal(SIGALRM, alarm_handler);
		alarm(5);
		// Executing the CGI script using execve system call
		execve(this->_cgi_interpreter.c_str(), args, env);
		exit(1);
	}
	else
	{
		// In parent process:
		// Closing unused ends of the pipes
		close(pipeIn[0]);
		close(pipeOut[1]);
		close(pipeIn[1]);
		_pid = pid;
		_cgi_fd_out = pipeOut[0];
	}

	// Freeing the memory used by environment variables
	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;

	// Exiting the child process
	if (!_pid)
		exit(0);
}

/*
*  @brief   Used to return the cgi code status from the request handler
*  @param   void
*  @return  int, the status code, 0 is fine
*/
int	CgiHandler::getStatus() const
{
	return this->_status;
}

std::string	CgiHandler::getBody() const
{
	return this->_body;
}
