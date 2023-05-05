/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ISocket.class.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlafont <tlafont@student.42angouleme.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/10 11:37:07 by tlafont           #+#    #+#             */
/*   Updated: 2023/04/25 14:28:53 by tlafont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SOCKET_HPP
#define SOCKET_HPP

// Includes

#include <iostream>
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <unistd.h>

// class interface
class ISocket
{
	public:
	    //---- canonical form ----//
			// constructors //
				// overload constructor
		ISocket(int dom, int serv, int protoc, int port, std::string &host);
			// destructor //
	    virtual ~ISocket(void);
			
	    //---- getter methods ----//
		int					getSocketFd() const;
		int					getConnection() const;
		struct sockaddr_in	getAddress() const;

	    //---- member methods ----//
				// virtual function to connect to a network
				//(at define for a bindSocket or a listenSocket)
		virtual int	connectToNetwork(int sock, struct sockaddr_in addr) const = 0;

				// connection test establishment
		void	testConnection(int to_test, std::string const &err);

	protected:
		int					_sock_fd;
		int					_connec;
		struct sockaddr_in	_addr;

	private:
    	ISocket(void);
	    ISocket(const ISocket& rhs);
	    ISocket  &operator=(const ISocket& rhs);
};

#endif
