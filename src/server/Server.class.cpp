/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amorel <amorel@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 08:29:43 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/03 18:24:43 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server/Server.class.hpp"

/*
*  @brief	Default constructor of the class Server.
*			Initialize the server without any parameters
*  @param	void
*  @return	void
*/
Server::Server():	_socket(NULL),
					_port(2424),
					_host("0.0.0.0"),
					_server_name("webserv"),
					_config(Parsing("conf/config_default.ini"))
{
}

/*
*  @brief	Overload constructor of the class ISocket.
*			Initialize the server with parameters
*  @param	int
*  @return	void
*/
Server::Server(Parsing &config, int n_serv)
{
	this->_port = atoi(config.getNServer(n_serv)->getPort().c_str());
	this->_nb_server = n_serv;
	this->_host = config.getNServer(n_serv)->getIp();
	this->_config = config;
	this->_server_name = config.getNServer(n_serv)->getServerName();
}

/*
*  @brief	copy constructor of the class Server.
*			Initialize the server with another Aserver
*  @param	Server &
*  @return	void
*/
Server::Server(Server const &rhs)
{
	*this = rhs;
}

/*
*  @brief	Assignment operator.
*			Copy server with another server
*  @param	Server &
*  @return	Server &
*/
Server	&Server::operator=(Server const &rhs)
{
	this->_port = rhs._port;
	this->_host = rhs._host;
	this->_server_name = rhs._server_name;
	this->_config = rhs._config;
	this->_new_socket = rhs._new_socket;
	return (*this);
}

/*
*  @brief	Getter socket.
*			access to the socket datas
*  @param	void
*  @return	ListenSocket *
*/
ListenSocket	*Server::getSocket() const
{
	return (this->_socket);
}

/*
*  @brief	Getter file descriptor socket.
*			Access to the fd of the socket 
*  @param	void
*  @return	int
*/
int	Server::getSocketFd() const
{
	return (this->_socket->getSocketFd());
}

/*
*  @brief	methode launch.
*			launch the methods accepter, handler and responder for communication 
*  @param	void
*  @return	bool
*/
bool	Server::launch()
{
	this->_socket = new ListenSocket(PF_INET, SOCK_STREAM, 0, this->_port, this->_host, 10);
	if (this->_socket->getSocketFd() == -1 || this->_socket->getConnection() == -1)
	{	
		delete this->_socket;	
		this->_socket = NULL;
		return false;
	}
	// set non_blocking the socket
	int setting = fcntl(this->_socket->getSocketFd(), F_SETFL, O_NONBLOCK);
	if (setting == -1)
	{
		std::string	color = "\033[1;31m\tError on ";
		this->displayTimestamp(color + this->_server_name + "\033[0m");
		this->displayTimestamp("\033[1;31m\tError: set non-blocking ListenSocket.\033[0m");
		throw std::runtime_error(strerror(errno));
	}
	return true;
}

/*
*  @brief	Create a ComSocket object.
*			create ComSocket form server socket fd and add in array of communications
*  @param	int
*  @return	int
*/
int	Server::createNewCom()
{
	ComSocket	*newCom = new ComSocket(this->_socket->getSocketFd(), this->_config);
	this->_all_com.push_back(newCom);
	return (newCom->getFdSocket());
}

/*
*  @brief	Management communication.
*			Manage the communications for the server
*  @param	void
*  @return	void
*/
void	Server::comManagement(Manager &manager)
{
	std::vector<ComSocket *>::iterator	it = this->_all_com.begin();
	while (it != this->_all_com.end())// No loop here, or more than one recv per select.
	{
		ComSocket	*com = *it;
		// check fd comSocket is in fds read list
 		if (FD_ISSET(com->getFdSocket(), manager.getListReadFd()))
		{
			this->displayTimestamp("\trequest being received.");
			if (com->isReceived() == true)
			{
				this->displayTimestamp("\tdemand well received.");
				FD_SET(com->getFdSocket(), manager.getListTmpWriteFd());
			}
		}
		// check if fd comSocket is in fds write list
		if (FD_ISSET(com->getFdSocket(), manager.getListWriteFd()) ||
			FD_ISSET(com->getCgiOutFd(), manager.getListReadFd()))
		{
			bool isCgi = false;
			if (!com->isCgi())
				this->displayTimestamp("\trequest being processed.");
			// parsing string request received
			com->parseRequest(this->_nb_server);
			try
			{
				//set the response, cgi or normal
				if (com->isCgi())
				{
					// come here second time, if it is cgi
					com->checkCgi();
					isCgi = com->isCgi();
					if (!isCgi)
						com->setCgiResponse();
				}
				else
				{
					// will come here first and set a normal response or a cgi
					isCgi = com->setResponse(this->_nb_server);
					// if cgi, add the fd to the readfds
					if (isCgi)
						FD_SET(com->getCgiOutFd(), manager.getListTmpReadFd());
					this->displayTimestamp("\tresponse under construction.");
				}
				if (!isCgi)
				{
					// send the response if normal or after cgi completed
					com->sendResponse();
					this->displayTimestamp("\treply sent to the client.");
					// reset the request and response for reuse
					com->clear();
				}
			}
			catch (std::exception &e)
			{
				com->setIsOpen(false);
				this->displayTimestamp("\033[1;31m\tError: response not properly established.\033[0m");
				std::string	color1 = "\033[1;31m\t";
				std::string	color2 = "\033[0m";
				this->displayTimestamp(color1 + e.what() + color2);
			}
			// supp the fds on list tmp write fds
			if (!isCgi)
				FD_CLR(com->getFdSocket(), manager.getListTmpWriteFd());
		}
		// check if com socket fd is closed
		if (!com->getIsOpen())
		{
			// supp the fd on tmp read and write list fds and in array of all connections
			FD_CLR(com->getFdSocket(), manager.getListTmpWriteFd());
			FD_CLR(com->getFdSocket(), manager.getListTmpReadFd());
			manager.getMapConnect()->erase(com->getFdSocket());
			// supp communication of array of comm
			delete *it;
			if (it != this->_all_com.end())
				this->_all_com.erase(it);
			// check for stop this communication
			if (this->_all_com.empty())
				break;
			else
			{
				// return at the begin of array of communications
				it = this->_all_com.begin();
				continue;
			}
		}
		it++;
	}
}

/*
*  @brief	display timestamp.
*			Set the msg and date hour
*  @param	std::string const &
*  @return	void
*/
void	Server::displayTimestamp(std::string const &event)
{
	std::time_t t = std::time(NULL);

	std::tm now = *localtime(&t);
	std::cout << "[\033[1;34m" << (now.tm_year + 1900) << std::setfill('0')
		<< "\033[0m/\033[1;34m"
		<< std::setw(2) << now.tm_mon + 1 << "\033[0m/\033[1;34m"
		<< std::setw(2) << now.tm_mday << " \033[1;33m"
		<< std::setw(2) << now.tm_hour << "\033[0m:\033[1;33m"
		<< std::setw(2) << now.tm_min << "\033[0m:\033[1;33m"
		<< std::setw(2) << now.tm_sec << "\033[0m] "
		<< event <<std::endl;
}

/*
*  @brief	clear and free server attribute.
*			
*  @param	void
*  @return	void
*/
void	Server::closeServer()
{
	std::vector<ComSocket *>::iterator	it = this->_all_com.begin();
	std::vector<ComSocket *>::iterator	ite = this->_all_com.end();
	for (; it != ite; it++)
	{
		close((*it)->getFdSocket());
		if ((*it)->getIsOpen())
			delete (*it);
	}
	delete this->_socket;
}

/*
*  @brief	Destructor.
*		   destroy the server and parameters
*  @param	void
*  @return	void
*/
Server::~Server()
{
}
