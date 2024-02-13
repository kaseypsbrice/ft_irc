#include "irc.hpp"

// leave channels
// PART #test,#test2 :going to lunch
void Server::command_part(t_cmd cmd)
{
	std::vector<std::string> channels = get_channels(cmd.message);
	std::string reason = get_reason(cmd.message);

	if (channels.empty())
	{
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(cmd.client->get_nick(), cmd.name));
		return ;
	}
	std::vector<std::string>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = get_channel(*it);
		if (channel == NULL)
		{
			cmd.client->set_writebuf(ERR_NOSUCHCHANNEL(cmd.client->get_nick(), *it));
			continue ;
		}
		if (!channel->get_client_by_fd(cmd.client->get_client_fd()))
		{
			cmd.client->set_writebuf(ERR_NOTONCHANNEL(cmd.client->get_nick(), *it));
			continue ;
		}
		// broadcast to every client in channel
		channel->broadcast_string(RPL_PART(user_id(cmd.client->get_nick(), cmd.client->get_user()), *it, reason));
		channel->remove_client(cmd.client);
	}
}