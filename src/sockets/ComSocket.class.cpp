/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComSocket.class.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlafont <tlafont@student.42angouleme.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 15:08:12 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/02 17:57:27 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sockets/ComSocket.class.hpp"

/*
*  @brief   Overload constructor.
*			Initialize the socket with parameters
*  @param   int
*  @return  void
*/
ComSocket::ComSocket(int fd, Parsing const &config):	_addr(),
														_fd_com(),
														_is_open(true),
														_is_send(false),
														_config(config)
{
	// Accept a connection on a socket
	socklen_t	size = sizeof(this->_addr);
	this->_fd_com = accept(fd, (struct sockaddr*)&this->_addr, &size);
	testConnection(this->_fd_com, std::string("\tError: communication not accept."));
	// set non_bolcking the socket
	if (fcntl(this->_fd_com, F_SETFL, O_NONBLOCK) == -1)
		testConnection(-1, "\tError: set non-blocking ComSocket");
	// set options in socket for re-use address
	int x = 1;
	int opt = setsockopt(this->_fd_com, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x));
	testConnection(opt, std::string("\tError: set option Comsocket."));
}

/*
*  @brief   Copy constructor.
*			copy other socket in this socket
*  @param   ComSocket &
*  @return  void
*/
ComSocket::ComSocket(ComSocket const &rhs)
{
	*this = rhs;
}

/*
*  @brief   Assignment operator.
*			copy other socket in this socket
*  @param   ComSocket &
*  @return  ComSocket &
*/
ComSocket	&ComSocket::operator=(ComSocket const &rhs)
{
	(void)rhs;
	return (*this);
}

/*
*  @brief	Get string IPP address .
*			return a string with address and port 
*  @param	void
*  @return	std::string
*/
//std::string	ComSocket::getIPP() const
//{
//	std::string	host = inet_ntoa(this->_addr.sin_addr);
//	int			port = htons(this->_addr.sin_port);
//	std::ostringstream oss;
//	oss << port;
//	return (host + ':' + oss.str());
//}

/*
*  @brief	Get fd.
*			return the file descriptor of ComSocket
*  @param	void
*  @return	int
*/
int	ComSocket::getFdSocket() const
{
	return (this->_fd_com);
}

/*
*  @brief	Getter for _is_open.
*			Get the comsocket is open or not
*  @param	void
*  @return	bool
*/
bool	ComSocket::getIsOpen() const
{
	return (this->_is_open);
}

/*
*  @brief	Setter for _is_open.
*			set the comsocket at open or not
*  @param	bool
*  @return	void
*/
void	ComSocket::setIsOpen(bool open)
{
	this->_is_open = open;
}

int	ComSocket::getCgiOutFd() const
{
	return this->_request.getCgiOutFd();
}

bool	ComSocket::isCgi() const
{
	return this->_request.isCgi();
}

void	ComSocket::checkCgi()
{
	this->_request.checkCgi();
}

/*
*  @brief	tester reception request.
*			Confirm  if request is received
*  @param	void
*  @return	bool
*/
bool ComSocket::isReceived()
{
	int		byteCount = 0;
	char	buffer[BUFSIZE];
	bool	isComplete = false;

	// Receive data from client
	byteCount = recv(this->_fd_com, buffer, BUFSIZE, 0);
	// Check if recv not failed
	if (byteCount == -1)
	{
		this->displayTimestamp("\033[1;31mError: receive request fail.\033[0m");
		return (true);
	}
	// Check if recv is complete
	else if (byteCount < BUFSIZE && byteCount != 0)
	{
		std::string	tmp;
		buffer[byteCount] = '\0';
		int i = 0;
		// Record data on a string byte by byte
		while (i < byteCount)
		{
			tmp += buffer[i];
			this->_received += buffer[i++];
		}
		if (tmp.rfind("\r\n\r\n") == this->_received.size() - 4)
		{
			isComplete = true;
		}
		std::string check = buffer;
		// Check if a file is waiting for upload
		if (check.find("boundary") != std::string::npos && isComplete)
			return (false);
		// Cherck if file is upload
		else if (!isComplete && this->_received.rfind("--\r\n") == this->_received.size() - 4)
			return (true);
		// check if we have an end of header whitout upload file
		else if (this->_received.rfind("\r\n\r\n") != this->_received.size() - 4 && !isComplete)
		{
			// check if post CGI request is full
			if (this->_received.find("Content-Type: application") != std::string::npos)
				return (true);
			return (false);
		}
		return (true);
	}
	// Check if no data in entrance
	else if (byteCount == 0)
	{
		return (true);
	}
	else
	{
		// record data in a string for continue receive
		buffer[byteCount] = '\0';
		int i = 0;
		// Record data on a string byte by byte
		while (i < byteCount)
			this->_received += buffer[i++];
	}
	return (false);
}

/*
*  @brief	calling for parsing request.
*  @param	void
*  @return	void
*/
void	ComSocket::parseRequest(int nServer)
{
	int bodyLimit = 0;
	std::string str = this->_config.getNServer(nServer)->getClientBodyLimit();
	std::istringstream(str) >> bodyLimit;
	this->_request.parsing(this->_received, bodyLimit);
}


//std::string	ComSocket::getRequest() const
//{
//	return (this->_received);
//}


void	ComSocket::setCgiResponse()
{
	CgiHandler	  *ch = _request.getCgiHandler();
	RequestHandler  *rh = _request.getRequestHandler();
	if (ch->getStatus())
	{
		rh->setStatusCode(500);
		std::string path = rh->getErrorPagePath();
		rh->setContentType(path);
		rh->setBody(rh->readContent(path));
	}
	else
		rh->setBody(ch->getBody());
	this->_response.buildResponse(*rh);
}

/*
*  @brief	set the response.
*			prepare the response
*  @param	void
*  @return	void
*/
bool	ComSocket::setResponse(int const nb_serv)
{
	//to modify when parsing is done
	std::string	request(this->_received);

	RequestHandler *rh = new RequestHandler(&this->_request, this->_config.getNServer(nb_serv));
	rh->run();
	this->_request.setRequestHandler(*rh);
	if (this->_request.isCgi())
		return true;
	else
	{
		this->_response.buildResponse(*rh);
		return false;
	}
}

/*
*  @brief	sent the response.
*			sent the response to the client
*  @param	void
*  @return	void
*/
void	ComSocket::sendResponse()
{
	unsigned long	toSend = this->_response.getResponse().size();
	long			ret = 0;
	// sending response
	ret = send(this->_fd_com, &(this->_response.getResponse().c_str()[0]), toSend, 0);
	if (ret == -1)
		throw std::runtime_error(strerror(errno));
	else if (ret >= 0)
		this->_is_send = true;
}

/*
*  @brief	reset all attributs.
*			clear and reset all for a new communication
*  @param	void
*  @return	void
*/
void	ComSocket::clear()
{
	//to modify when Respose class is implemented
	if (this->_is_send)
	{
		close(this->_fd_com);
		this->_is_open = false;
		return ;
	}
	else if (this->_is_open == false)
		return ;
	this->_is_open = true;
	this->_request.getRequest().clear();
	this->_response.getResponse().clear();
}

/*
*  @brief	Test _fd_com .
*			Confirm  data properly established
*  @param	int, std::string
*  @return	void
*/
void	ComSocket::testConnection(int data, std::string const &err)
{
	if (data < 0)
	{
		this->displayTimestamp("\033[1;31m" + err + "\033[0m");
		throw std::runtime_error(strerror(errno));
	}
}

/*
*  @brief	display timestamp.
*		   Set the msg and date hour
*  @param	std::string const &
*  @return	void
*/
void	ComSocket::displayTimestamp(std::string const &event)
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
*  @brief   Destructor.
*		   Destroy all member objects
*  @param   void
*  @return  void
*/
ComSocket::~ComSocket()
{
	close(this->_fd_com);
	this->_fd_com = -1;
}
