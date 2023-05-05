/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComSocket.class.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlafont <tlafont@student.42angouleme.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 15:14:20 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/02 13:10:25 by tlafont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef COMSOCKET_HPP
# define COMSOCKET_HPP

// includes
#include <arpa/inet.h>
#include <sys/socket.h>
#include "request/Request.class.hpp"

#define BUFSIZE 65535

class Request;

class ComSocket
{
	public:
		//---- canonical form ----//
			// constructors //
				// overload constructor
		ComSocket(int fd, Parsing const &config);
			// destructor //
		~ComSocket();
		//---- getters methods ----//
//        std::string	getRequest() const;
//		std::string	getIPP() const;
		int			getFdSocket() const;
		bool		getIsOpen() const;

		//---- setter methods ----//
		void	setIsOpen(bool open);

		//---- member methods ----//
		bool		isReceived();
		void		parseRequest(int nServer);
        void        setCgiResponse();
		bool		setResponse(int const nb_serv);
		void		sendResponse();
		void		clear();

        //---- cgi ----//
        bool        isCgi() const;
        int			getCgiOutFd() const;
        void        checkCgi();

	private:
		//---- member object ----//
		struct sockaddr_in					_addr;
		int									_fd_com;
		bool								_is_open;
		bool								_is_send;
		std::string							_received;
		Response							_response;
		Request								_request;
		Parsing								_config;

		//---- private methods ----//
			// canonical form
		ComSocket();
		ComSocket(ComSocket const &rhs);
		ComSocket	&operator=(ComSocket const &rhs);
			// error test
		void	testConnection(int data, std::string const &err);
			// display msg in server
		void	displayTimestamp(std::string const &event);

};

#endif
