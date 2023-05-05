/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlafont <tlafont@student.42angouleme.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 15:58:25 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/02 17:26:01 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response/Response.class.hpp"

/*
*  @brief   Default constructor.
*  @param   void
*  @return  void
*/
Response::Response() {}

/*
*  @brief   Destructor.
*		   Destroy all member objects
*  @param   void
*  @return  void
*/
Response::~Response() {}

/*
*  @brief   Assignment operator.
*		   copy other response in this response
*  @param   Response &
*  @return  Response &
*/
Response	&Response::operator=(Response const &rhs)
{
	(void)rhs;
	return (*this);
}

/*
*  @brief   Getter.
*  @param   void
*  @return  response string
*/
std::string	Response::getResponse() const
{
	return this->_response;
}

/*
*  @brief get the current date
*  @param   void
*  @return  std::string
*/
std::string Response::getDate()
{
	std::string date;
	time_t now_time = std::time(NULL);
	struct tm *now = std::localtime(&now_time);
	std::stringstream ss;
	std::string weekday[7] = {"Mon",
							  "Tue",
							  "Wed",
							  "Thu",
							  "Fri",
							  "Sat",
							  "Sun"};

	std::string month[12] = {"Jan",
							 "Feb",
							 "Mar",
							 "Apr",
							 "May",
							 "Jun",
							 "Jul",
							 "Aug",
							 "Sep",
							 "Oct",
							 "Nov",
							 "Dec"};

	date += weekday[now->tm_wday];
	date += ", ";
	ss << now->tm_mday;
	date += ss.str();
	ss.clear();
	ss.str("");
	date += " ";
	date += month[now->tm_mon];
	date += " ";
	ss << now->tm_year;
	date += ss.str();
	ss.clear();
	ss.str("");
	date += " ";
	ss << now->tm_hour;
	date += ss.str();
	ss.clear();
	ss.str("");
	date += ":";
	ss << now->tm_min;
	date += ss.str();
	ss.clear();
	ss.str("");
	date += ":";
	ss << now->tm_sec;
	date += ss.str();
	ss.clear();
	ss.str("");
	date += " ";
	ss << now->tm_zone;
	date += ss.str();
	ss.clear();
	ss.str("");

	return date;
}


/*
*  @brief   Build response to send.
*			Use the requestHandler class with the parsed request to get the responses element
*			Compose the response with the requestHandler getters (status code, content type, etc..)
*  @param   std::string & (to modify when Request class established)
*  @return  void
*/
void	Response::buildResponse(RequestHandler &rh)
{

	std::string header_sep = "\r\n";
	std::string body_sep = "\r\n\r\n";

	// HEADER
	this->_response = "";
	this->_response += "HTTP/1.1 ";
	this->_response += rh.getStatusCodeString();
	this->_response += header_sep;
	// Check if there is a redirection
	if (rh.getStatusCode() == 302) 
	{
		this->_response += "Location: " + rh.getRedirectLocation();
		this->_response += header_sep;
	}
	else
	{
		this->_response += "Content-Type: " + rh.getContentType() + "; charset=UTF-8";
		this->_response += header_sep;
	}
	this->_response += "Date: " + this->getDate();
	this->_response += header_sep;
	this->_response += "Server: " + rh.getServerName();

	// BODY
	this->_response += body_sep;
	this->_response += rh.getBody();
}
