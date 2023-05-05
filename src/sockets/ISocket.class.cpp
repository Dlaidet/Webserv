/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ISocket.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlafont <tlafont@student.42angouleme.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/10 12:06:39 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/03 09:23:29 by tlafont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sockets/ISocket.class.hpp"

/*
*  @brief	Default constructor of the class ISocket.
*			Initialize the socket without any parameters
*  @param	void
*  @return	void
*/
ISocket::ISocket()
{
}

/*
*  @brief	Overload constructor of the class ISocket.
*			Initialize the socket with parameters
*  @param	int , int , int , int , unsigned long
*  @return	void
*/
ISocket::ISocket(int dom, int serv, int protoc, int port, std::string &host)
{
	//set values in address struct for connection
	this->_addr.sin_family = dom;
		//set octets in order for nertwork (htons, htonl)
	this->_addr.sin_port = htons(port);
	// set host address
	if (host != "0.0.0.0" || host != "localhost")
		this->_addr.sin_addr.s_addr = inet_addr(host.c_str());
	else
		this->_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//establish the socket and test
	this->_sock_fd = socket(dom, serv, protoc);
	//for test
	testConnection(this->_sock_fd, std::string("\tError: fd socket."));
	int x = 1;
	int opt = setsockopt(this->_sock_fd, SOL_SOCKET, SO_REUSEPORT, &x, sizeof(x));
	testConnection(opt, std::string("\tError: set option socket."));
}

/*
*  @brief	Copy  constructor.
*			Initialize the socket with other socket
*  @param	ISocket &
*  @return	void
*/
ISocket::ISocket(const ISocket& rhs)
{
	*this = rhs;
}

/*
*  @brief	Assignment operator.
*			Copy other socket in this socket
*  @param	ISocket &
*  @return	Isocket &
*/
ISocket  &ISocket::operator=(const ISocket& rhs)
{
	(void)rhs;
	return (*this);
}

/*
*  @brief	Test _sock_fd or _connec.
*			Confirm data properly established
*  @param	int
*  @return	void
*/
void	ISocket::testConnection(int data, std::string const &err)
{
	// If the data is less than 0, indicating an error...
	if (data < 0)
	{
		// Get the current time.
		std::time_t t = std::time(NULL);
		std::tm now = *localtime(&t);
		
		// Print an error message with the current time, error message, and errno value.
		std::cerr << "[\033[1;34m" << (now.tm_year + 1900) << std::setfill('0')
			<< "\033[0m/\033[1;34m"
			<< std::setw(2) << now.tm_mon + 1 << "\033[0m/\033[1;34m"
			<< std::setw(2) << now.tm_mday << " \033[1;33m"
			<< std::setw(2) << now.tm_hour << "\033[0m:\033[1;33m"
			<< std::setw(2) << now.tm_min << "\033[0m:\033[1;33m"
			<< std::setw(2) << now.tm_sec << "\033[1;31m] "
			<< err << "\033[0m" <<std::endl;

		// Throw a runtime_error exception with the strerror value.
	//	throw std::runtime_error("");
	}
}

/*
*  @brief	Getter for private attribut socket.
*			Return the socket associate
*  @param	void
*  @return	int
*/
int	ISocket::getSocketFd() const
{
	return (this->_sock_fd);
}

/*
*  @brief	Getter for private attribut connection.
*			Return the connection associate
*  @param	void
*  @return	int
*/
int	ISocket::getConnection() const
{
	return (this->_connec);
}

/*
*  @brief	Getter for private attribut address.
*			Return the structur address associate
*  @param	void
*  @return	struct sockaddr_in
*/
struct sockaddr_in	ISocket::getAddress() const
{
	return (this->_addr);
}

/*
*  @brief	Destructor
*			Destroy all member objects
*  @param	void
*  @return	void
*/
ISocket::~ISocket()
{
	if (this->_sock_fd > 2)
		close(this->_sock_fd);
}
