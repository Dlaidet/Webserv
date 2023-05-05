/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BindSocket.class.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlafont <tlafont@student.42angouleme.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/10 14:30:23 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/03 07:59:49 by tlafont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sockets/BindSocket.class.hpp"

/*
*  @brief   Overload constructor.
*			Initialize the socket with parameters
*  @param   int, int, int, int , unsigned long
*  @return  void
*/
BindSocket::BindSocket(int dom, int serv, int protoc, int port, std::string &host):
ISocket(dom, serv, protoc, port, host)
{
	// Establish the connection to the network and test it.
	// Attempt up to 6 times in case of failure.
	if (this->_sock_fd != -1)
	{
		for (int i = 0; i < 6; i++)
		{
			// Call the connectToNetwork function to establish the connection.
			// Pass in the socket file descriptor and address info as arguments.
			this->_connec = connectToNetwork(this->_sock_fd, this->_addr);
			// If the connection failed...
			if (this->_connec == -1)
			{
				std::string	err = "\tError: binding (";
				// Print an error message with the errno and strerror values.
				if (i != 5)
					testConnection(-1, err + strerror(errno) + ")");
				// If we've reached the maximum number of attempts, handle the error.
				else
					testConnection(this->_connec, std::string("\tError: name affectation socket."));
			}
			// If the connection was successful, break out of the loop.
			else
				break;
		}
	}
}

/*
*  @brief   Assignment operator.
*			copy other socket in this socket
*  @param   BindSocket &
*  @return  BindSocket &
*/
BindSocket	&BindSocket::operator=(BindSocket const &rhs)
{
	(void)rhs;
	return (*this);
}

/*
*  @brief   Method for establish server connection.
*			Set the connection for server
*  @param   int , struct sockaddr_in
*  @return  int
*/
int BindSocket::connectToNetwork(int sock, struct sockaddr_in addr) const
{
	return (bind(sock, (struct sockaddr *)&addr, sizeof(addr)));
}

/*
*  @brief   Destructor.
*			Destroy all member objects
*  @param   void
*  @return  void
*/
BindSocket::~BindSocket()
{
}
