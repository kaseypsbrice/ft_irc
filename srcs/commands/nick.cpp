#include "irc.hpp"

static bool invalid_nick(std::string nick)
{
	if (nick[0] == '$' || nick[0] == ':' || nick[0] == '#')
		return true;
	if (nick.find_first_of(" ,*?!@.") != std::string::npos)
		return true;
	return false;
}

void Server::command_nick(t_cmd cmd)
{
	// todo check for duplicate nicks
	if (cmd.message.size() == 0)
	{
		// empty nick reply
		return ;
	}
	else if (invalid_nick(cmd.message))
	{
		// invalid nick reply
		return ;
	}
	cmd.client->set_nick(cmd.message);
	cmd.client->set_nick_registered(true);
	std::cout << "Nickname set: " << cmd.client->get_nick() << std::endl;
	// default reply
}
