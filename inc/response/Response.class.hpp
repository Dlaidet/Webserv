/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.class.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaadoun <mdaadoun@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 15:58:25 by mdaadoun          #+#    #+#             */
/*   Updated: 2023/05/03 18:20:49 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <fstream>
#include <string>

#include "request/RequestHandler.class.hpp"
#include "parsing/Parsing.class.hpp"

class Request;
class CgiHandler;
class RequestHandler;

class Response
{
	public:
        // constructor and destructor
		Response();
		~Response();

		//---- getter ----//
        std::string     getDate();
        std::string     getResponse(void) const;

		//---- member methods ----//
		void	        buildResponse(RequestHandler &rh);
	private:
        Response	    &operator=(Response const &rhs);
		std::string		_response;
};

#endif
