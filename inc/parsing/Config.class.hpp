/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.class.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amorel <amorel@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 14:56:44 by amorel            #+#    #+#             */
/*   Updated: 2023/05/04 11:47:18 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <map>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <dirent.h>

class Config
{
	private:
		std::string													_ip;
		std::string													_port;
		std::string													_listen;
		std::string													_root;
		std::string													_index;
		std::string													_autoIndex;
		std::string													_serverName;
        std::string													_clientBodyLimit;
		std::map<std::string, std::string>							_errorPages;
		std::map<std::string, std::map<std::string, std::string> >	_locations;
        std::map<std::string, std::map<std::string, std::string> >	_cgi;

	public:
		Config();
		Config(const Config &copy);
		~Config();

        Config   &operator=(Config const &copy);

		bool														empty;
		std::string													getIp() const;
		std::string													getPort() const;
		std::string													getListen() const;
		std::string													getRoot() const;
		std::string													getIndex() const;
		std::string													getAutoIndex() const;
		std::string													getServerName() const;
		std::string													getClientBodyLimit() const;
		std::map<std::string, std::string>							getErrorPages() const;
		std::map<std::string, std::map<std::string, std::string> >	getLocations() const;
		std::map<std::string, std::map<std::string, std::string> >	getCgi() const;

		void	setIp(const std::string &ip);
		void	setPort(const std::string &port);
		void	setListen(const std::string &listen);
		void	setRoot(const std::string &root);
		void	setIndex(const std::string &index);
		void	setAutoIndex(const std::string &autoIndex);
		void	setServerName(const std::string &serverName);
		void	setClientBodyLimit(const std::string &clientBodyLimit);
		void	setErrorPages(const std::string &errorPage);
		void	setLocations(std::ifstream &file, std::string buf);
        void	setCgi(std::ifstream &file, std::string buf);

		void	checkIP();
		void	checkPort();
		void	checkRoot();
		void	checkAutoIndex();
		void	checkServerName();
		void	checkClientBodyLimit();
		void	checkErrorPages();

		void	printErrorPages();
		void	printLocations();
        void	printCgi();

		void	clear();
		void	insertLocation(std::string location, std::map<std::string, std::string> tmp);

		class ErrorFileException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		
		class ErrorPortOutOfBounds : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		class ErrorBadArgument : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
};

#endif
