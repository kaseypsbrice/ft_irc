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
# define VALID_COMMANDS 16

# include "Server.hpp"

std::string	get_reason(std::string msg);
bool is_alpha(std::string str);
std::string	get_channel_name(std::string msg_to_parse);
std::vector<std::string> get_channels(std::string msg);

#endif