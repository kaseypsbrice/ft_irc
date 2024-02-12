# include "irc.hpp"

static void broadcast_quit(Channel &channel, std::string nick, std::string user, std::string reason)
{
	std::map<const int, Client *> client_map = channel.get_client_map();
	std::map<const int, Client *>::iterator it;

	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		it->second->set_writebuf(RPL_QUIT(user_id(nick, user), reason));
	}
}

void Server::command_quit(t_cmd cmd)
{
	std::string reason = get_reason(cmd.message);
	std::string nick = cmd.client->get_nick();
	std::string user = cmd.client->get_user();

	std::map<std::string, Channel>::iterator it;
	cmd.client->set_writebuf(RPL_QUIT(user_id(nick, user), reason));
	for (it = _channel_map.begin(); it != _channel_map.end(); it++)
	{
		if (it->second.remove_client(cmd.client))
			broadcast_quit(it->second, nick, user, reason);
	}
	cmd.client->set_to_remove(true);
}