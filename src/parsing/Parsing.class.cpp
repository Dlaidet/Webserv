/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amorel <amorel@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 15:28:36 by amorel            #+#    #+#             */
/*   Updated: 2023/05/03 17:28:18 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing/Parsing.class.hpp"

/*
*  @brief	Default constructor of the class Parsing.
*		   Launch Parser and checker with "config_default.ini".
*  @param	void
*  @return	void
*/
Parsing::Parsing()
{

}

/*
*  @brief	Overload constructor of the class Parsing.
*		   Launch Parser and checker with argument path.
*  @param	const std::string &path
*  @return	void
*/
Parsing::Parsing(const std::string &file)
{
	Parsing::parseConfig(file);
	Parsing::checkConfig();
}

/*
*  @brief	Copy constructor of the class Parsing.
*		   Initialize the Parsing with Parsing received as argument.
*  @param	const Parsing &copy
*  @return	void
*/
Parsing::Parsing(const Parsing &copy)
{
	this->_servers = copy.getServers();
}

/*
*  @brief	Default deconstructor of the class Parsing.
*  @param	void
*  @return	void
*/
Parsing::~Parsing()
{

}

/*
*  @brief   Assignment operator.
*		   copy other Parsing in this Parsing
*  @param   Parsing &
*  @return  Parsing &
*/
Parsing &Parsing::operator=(const Parsing &copy)
{
	this->_servers = copy.getServers();
	return *this;
}

/*
*  @brief	Default constructor of the class Parsing.
*		   Initialize the config with default file "config_default.ini".
*  @param	const std::string &path
*  @return	void
*/
void	Parsing::parseConfig(const std::string &configfile)
{
	std::ifstream						file;
	std::string							buf;
	Config								tmp;
	std::map<std::string, std::string>	tmpmap;
	std::string::size_type				pos;
	std::string							key;
	std::string							value;
	bool								keyConfig = false;

	file.open(configfile.c_str());
	if (file.fail())
		throw(Config::ErrorFileException());
	while (getline(file, buf, '\n'))
	{
		pos = buf.find('=');
		if (buf == "[server]")
		{
			if (!keyConfig)
				keyConfig = true;
			if (tmp.empty)
				continue;
			if (tmp.getLocations().empty())
			{
				tmpmap.insert(std::pair<std::string, std::string>("allow_methods", ""));
				tmp.insertLocation("/", tmpmap);
				tmpmap.clear();
			}
			this->_servers.insert(_servers.end(), tmp);
			tmp.clear();
		}
		else if (buf.find('=') != std::string::npos && keyConfig)
		{
			key = buf.substr(0, pos);
			value = buf.substr(pos + 1);

			if (key == "server_name")
				tmp.setServerName(value);
			else if (key == "listen")
				tmp.setListen(value);
			else if (key == "root")
				tmp.setRoot(value);
			else if (key == "index")
				tmp.setIndex(value);
			else if (key == "autoindex")
				tmp.setAutoIndex(value);
			else if (key == "client_body_limit")
				tmp.setClientBodyLimit(value);
			else if (key.substr(0, 11) == "error_page_")
				tmp.setErrorPages(buf);
		}
		else if (buf.find("location:") != std::string::npos && keyConfig)
			tmp.setLocations(file, buf);
		else if (buf.find("cgi:") != std::string::npos && keyConfig)
			tmp.setCgi(file, buf);
	}
	if (file.eof())
	{
		if (tmp.getLocations().empty())
		{
			tmpmap.insert(std::pair<std::string, std::string>("allow_methods", ""));
			tmp.insertLocation("/", tmpmap);
			tmpmap.clear();
		}
		if (!tmp.empty)
			this->_servers.insert(_servers.end(), tmp);
	}
	file.close();
}

/*
*  @brief	Return attribute _servers;
*  @param	void
*  @return	std::vector<class Config>
*/
std::vector<class Config>	Parsing::getServers() const
{
	return _servers;
}

/*
*  @brief	Return number n Config member of attribute _servers.
*  @param	void
*  @return	class Config *
*/
class Config*	Parsing::getNServer(int n)
{
	return &(_servers.at(n));
}

/*
*  @brief	Launch all check function of Config Class member of attribute _servers.
*  @param	void
*  @return	void
*/
void	Parsing::checkConfig()
{
	std::vector<Config>::iterator	it;
	std::vector<Config>::iterator	its;

	if (_servers.empty())
		throw(Config::ErrorBadArgument());
	for (it = _servers.begin(); it != _servers.end(); it++)
	{
		it->checkRoot();
		it->checkIP();
		it->checkPort();
		it->checkAutoIndex();
		it->checkServerName();
		it->checkClientBodyLimit();
		it->checkErrorPages();
	}
	for (it = _servers.begin(); it != _servers.end(); it++)
	{
		for (its = it + 1; its != _servers.end(); its++)
		{
			if (it->getPort() == its->getPort())
			{
				throw(Config::ErrorBadArgument());
			}
		}
	}
}
