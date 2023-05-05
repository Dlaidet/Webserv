/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.class.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlafont <tlafont@student.42angouleme.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 15:18:51 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/03 17:35:26 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <algorithm>
# include <iomanip>

# include "enum.hpp"
# include "response/Response.class.hpp"
# include "cgi/CgiHandler.class.hpp"

class ComSocket;
class CgiHandler;

enum headerType
{
	ACCEPT_CHARSET,
	ACCEPT_LANGUAGE,
	ALLOW,
	AUTHORIZATION,
	CONNECTION,
	CONTENT_LANGUAGE,
	CONTENT_LENGTH,
	CONTENT_LOCATION,
	CONTENT_TYPE,
	DATE,
	HOST,
	LAST_MODIFIED,
	LOCATION,
	REFERER,
	REMOTE_USER,
	RETRY_AFTER,
	SERVER,
	TRANSFER_ENCODING,
	USER_AGENT,
	WWW_AUTHENTICATE
};

class Request
{
	public:
		//---- canonical form ----//
			// constructor //
		Request();

			// destructor //
		~Request();
		
		//---- getters ----//
        std::string 						getRequest() const;
		int									getStatus() const;
		std::string							getMethod() const;
		std::pair<std::string, std::string>	getUri() const;
		std::map<std::string, std::string>	getCgi() const;
		std::string							getBody() const;
		
		//---- member methods ----//
			// parsing the request received
		void 		parsing(std::string const &req, int bodyLimit);

        	//---- cgi ----//
        int			                            getCgiInFd() const;
        int			                            getCgiOutFd() const;
        bool                                    isCgi() const;
        void                                    checkCgi();
        void                                    setCgiHandler(CgiHandler& handler);
        void                                    setRequestHandler(RequestHandler& handler);
        CgiHandler                              *getCgiHandler();
        RequestHandler                          *getRequestHandler();

	private:
		//---- member objects ----//
			//for parse the original request
		std::string							_to_parse;
		std::string							_all_uri;
			//for comp the methods
		std::map<std::string, m_METHOD>		_methods;
			//for comp the headers
		std::map<std::string, headerType>	_headers;			
			// code status of request
		int									_status;
			// for record the header in request
		std::map<headerType, std::string>	_reqHeaders;
			// for record the method in header
		m_METHOD							_rec_method;
			// for record uri configuration
		std::pair<std::string, std::string>	_uri;
			// for record version
		std::pair<int, int>					_version;
			// for record all CGI params
		std::map<std::string, std::string>	_cgi;
			// for record body datas
		std::string							_body;
			//for record all headers when is parsed
		std::map<std::string, std::string>	_env;

        //---- cgi ----//
        CgiHandler                            *_cgiHandler[1];
        RequestHandler                        *_requestHandler[1];

		//---- assignement operator ----//
		Request	&operator=(Request const &rhs);

		//---- private methods ----//
			// init datas
		void	initMapMethods();
		void	initMapHeaders();
			// parsing helpers
		void	parseHeader(std::string const &req);
			// parsing and rec start-line
		void	parseStartLineRequest();
		void	recoveryMethods(size_t end_of_req, size_t &len);
		void	recoveryUri(size_t end_of_req, size_t len);
		void	recoveryVersion(size_t end_of_req, size_t &len);
			// parsing and rec headers protocols
		void	parseProtocolHeaders();
			// split URI for record CGI params
		void	parseUri();
			// parsing and record body content
		void	parseBody(size_t bodyLimit);
		void	parseCgi(std::string body);
			// record in _env
		void	setHeadersEnv(std::string const &header, std::string const &value);
			// display timestamp with error msg
		void	displayTimestamp(std::string const &event);
};

std::ostream	&operator<<(std::ostream &oss, Request const &req);

#endif
