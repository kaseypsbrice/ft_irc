/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kbrice <kbrice@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 18:06:10 by kbrice            #+#    #+#             */
/*   Updated: 2024/01/21 18:06:15 by kbrice           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: ./ft_irc [port] [password]" << std::endl;
		return (1);
	}
	Server server(argv[1], argv[2]);
	if (server.create_server())
		return (1);
	if (server.server_loop())
		return (1);
	return (0);
}