/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amorel <amorel@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 15:41:09 by fleblanc          #+#    #+#             */
/*   Updated: 2023/03/01 12:37:54 by tlafont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef TEST_HPP
#define TEST_HPP

#include <iostream>
#include <exception>
#include <string>
#include <cstdlib>

#include "sockets/BindSocket.class.hpp"
#include "sockets/ISocket.class.hpp"
#include "sockets/ListenSocket.class.hpp"
//#include "../inc/parsing/Parsing.hpp"
#include "cgi/CgiHandler.class.hpp"
#include "request/Request.class.hpp"
#include "request/RequestHandler.class.hpp"
#include "response/Response.class.hpp"
#include "manager/Manager.class.hpp"

void	parsingTest(void);
void	socketTest(void);
void	responseTest(void);
void	cgiTest(void);
void	requestTest(void);
void    handlerRequestTest(void);

#endif
