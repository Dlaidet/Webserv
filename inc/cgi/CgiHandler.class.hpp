/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.class.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaadoun <mdaadoun@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 16:13:31 by mdaadoun          #+#    #+#             */
/*   Updated: 2023/05/04 11:22:13 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <fstream>

#include "request/RequestHandler.class.hpp"

class RequestHandler;

class CgiHandler
{
	public:
		// default constructor and destructor
    	CgiHandler();
		CgiHandler(RequestHandler const &rh);
		~CgiHandler(void);

		// setters and getters
		void		setEnv(void);
		char**		getEnvChar();
		int			getStatus(void) const;
		int			getFdIn() const;
		int			getFdOut() const;
		std::string	getBody() const;
		bool		isActive() const;
		std::string	recomposeQueryString(std::map<std::string, std::string> &query);

		static void	alarm_handler(int signal_number);

    // main method of object, execute script and return body response as string
		void		executeCgi();
		void		checkCgi();

	private:

		pid_t								_pid;
		bool								_is_cgi_running;
		int									_cgi_fd_in;
		int									_cgi_fd_out;

		std::map<std::string, std::string>	_env;
		std::string							_script,
											_cgi_interpreter,
											_body,
											_method,
											_query_string;
		int									_status;
};

#endif
