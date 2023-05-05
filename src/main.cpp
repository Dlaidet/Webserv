/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amorel <amorel@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 10:31:27 by tlafont           #+#    #+#             */
/*   Updated: 2023/04/26 12:10:14 by tlafont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
* Simple implementation of a web server's main function that parses the configuration file, 
*	sets up the connections, and starts the management process.
*/ 
#include "main.hpp"

int main(int ac, char **av)
{
	if (ac < 3)
	{
		if (ac == 1)
		{
			try
			{
				// parsing config file 
				Parsing config("conf/config_default.ini");
				// construction du Manager
				std::cout << ASCIART << std::endl;
				Manager	manager(config);
				// init du manager
				manager.initConnections();
				// running manager
				manager.managementProcess();
			}
			catch (std::exception &e)
			{
				std::cout << "\033[1;31m" << e.what() << "\033[0m" << std::endl;
			}
		}
		else
		{
			try
			{
				// parsing config file 
				Parsing config(av[1]);
				// construction du Manager
				std::cout << ASCIART << std::endl;
				Manager	manager(config);
				// init du manager
				manager.initConnections();
				// running manager
				manager.managementProcess();
			}
			catch (std::exception &e)
			{
				std::cout << "\033[1;31m" << e.what() << "\033[0m" << std::endl;
			}
		}
	}
	else
		std::cerr << "\033[1;31mUsage: \033[0m./webserv [configuration file]" << std::endl;
    return 0;
}
