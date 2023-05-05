/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.class.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amorel <amorel@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 15:23:24 by amorel            #+#    #+#             */
/*   Updated: 2023/05/03 17:28:22 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef PARSING_HPP
#define PARSING_HPP

#include "../inc/parsing/Config.class.hpp"

class Parsing
{
	private:

		std::vector<class Config>	_servers;

	public:
	
		Parsing();
		Parsing(const std::string &file);
		Parsing(const Parsing &copy);
		~Parsing();

		Parsing &operator=(const Parsing &copy);

		std::vector<class Config>	getServers() const;
		class Config				*getNServer(int n);
		void						parseConfig(const std::string &file);
		void						checkConfig();
};

#endif
