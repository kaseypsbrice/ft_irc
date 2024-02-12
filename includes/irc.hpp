/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kbrice <kbrice@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 18:18:45 by kbrice            #+#    #+#             */
/*   Updated: 2024/01/21 18:19:26 by kbrice           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# define MESSAGE_BUF 2048
# define VALID_COMMANDS 17

# include "Server.hpp"

std::string	get_reason(std::string msg);

#endif