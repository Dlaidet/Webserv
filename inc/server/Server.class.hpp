/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.class.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amorel <amorel@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 08:31:33 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/04 11:46:01 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

//includes
#include "sockets/ListenSocket.class.hpp"
#include "sockets/ComSocket.class.hpp"
#include "parsing/Parsing.class.hpp"
#include "manager/Manager.class.hpp"

class Manager;

class Server
{
	public:
		//---- canonical form ----//
			// constructors //
				// copy constructor
		Server(Server const &rhs);
				// overload constructor
		Server(Parsing &config, int n_serv);
			// assignment operator //
		Server	&operator=(Server const &rhs);
			// destructor //
		~Server();
		
		//---- getter methods ----//
		ListenSocket	*getSocket() const;
		int				getSocketFd() const;
		
		//---- member methods ----//
		bool	launch();
		int		createNewCom();
		void	comManagement(Manager &manager);
		void	displayTimestamp(std::string const &event);
		void	closeServer();

	private:
		//---- member objects ----//
		ListenSocket				*_socket;
		std::vector<ComSocket *>	_all_com;
		int							_new_socket;
		int							_nb_server;
			// for config sockets
		int							_port;
		std::string					_host;
		std::string					_server_name;

			//config after parsing
		Parsing						_config;

		//---- canonical form ----//
				// default constructor
		Server();
};

#endif
