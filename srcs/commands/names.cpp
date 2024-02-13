#include "irc.hpp"

void Server::command_names(t_cmd cmd)
{
	std::vector<std::string> channels = get_channels(cmd.message);
	std::vector<std::string>::iterator it;
	std::string nick = cmd.client->get_nick();

	if (channels.empty())
	{
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(nick, cmd.name));
		return ;
	}
	for (it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = get_channel(*it);

		if (channel == NULL)
		{
			cmd.client->set_writebuf(RPL_ENDOFNAMES(nick, *it));
			continue ;
		}
		std::string symbol = channel->get_symbol();
		std::string members = get_list_of_members(channel);
		if (!members.empty())
			cmd.client->set_writebuf(RPL_NAMREPLY(nick, symbol, *it, members));
		cmd.client->set_writebuf(RPL_ENDOFNAMES(nick, *it));
	}
}