#include "irc.hpp"

static bool invalid_nick(std::string nick)
{
	if (nick[0] == '$' || nick[0] == ':' || nick[0] == '#')
		return true;
	if (nick.find_first_of(" ,*?!@.") != std::string::npos)
		return true;
	return false;
}

bool Server::is_nick_taken(std::string nick, const int client_fd)
{
	std::map<const int, Client>::iterator it;

	for (it = _client_map.begin(); it != _client_map.end(); it++)
	{
		if (it->second.get_nick() == nick && it->first != client_fd)
			return true;
	}
	return false;
}

void Server::command_nick(t_cmd cmd)
{
	if (cmd.message.size() == 0)
	{
		cmd.client->set_writebuf(ERR_NONICKNAMEGIVEN(cmd.client->get_nick()));
		return ;
	}
	else if (invalid_nick(cmd.message))
	{
		cmd.client->set_writebuf(ERR_ERRONEUSNICKNAME(cmd.client->get_nick(), cmd.message));
		return ;
	}
	else if (is_nick_taken(cmd.message, cmd.client->get_client_fd()))
	{
		cmd.client->set_writebuf(ERR_NICKNAMEINUSE(cmd.client->get_nick(), cmd.message));
		return ;
	}
	if (cmd.client->is_registered())
		cmd.client->set_old_nick(cmd.client->get_nick());
	else
		cmd.client->set_old_nick(cmd.message);
	cmd.client->set_nick(cmd.message);
	cmd.client->set_nick_registered(true);
	std::cout << "Nickname set: " << cmd.client->get_nick() << std::endl;
	if (cmd.client->is_registered())
		cmd.client->set_writebuf(RPL_NICK(cmd.client->get_old_nick(), cmd.client->get_user(), cmd.client->get_nick()));
	else if (cmd.client->is_user_registered())
	{
		if (!cmd.client->is_password_correct())
		{
			cmd.client->set_writebuf(ERR_PASSWDMISMATCH(cmd.client->get_nick()));
			cmd.client->set_to_remove(true);
			return ;
		}
		cmd.client->set_registered(true);
		cmd.client->set_writebuf(RPL_WELCOME(user_id(cmd.client->get_nick(), cmd.client->get_user()), cmd.client->get_nick()));
	}
}
