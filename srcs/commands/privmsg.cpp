#include "irc.hpp"

static std::string get_target(std::string msg)
{
	int delim;

	delim = msg.rfind(":");
	msg = msg.substr(0, delim);
	msg.erase(std::remove(msg.begin(), msg.end(), ' '), msg.end());
	return msg;
}

static std::string get_message(std::string msg)
{
	int delim;

	delim = msg.rfind(":");
	msg = msg.substr(delim);
	return msg;
}

// broadcast message to every client in channel except caller
void Server::broadcast_channel_message(Channel *channel, Client *client, std::string target, std::string message)
{
	std::map<const int, Client *> client_map = channel->get_client_map();
	std::map<const int, Client *>::iterator it;

	it = client_map.find(client->get_client_fd());
	if (it == client_map.end())
	{
		client->set_writebuf(ERR_NOSUCHNICK(client->get_nick(), target));
		return ;
	}

	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (it->second->get_client_fd() == client->get_client_fd())
			continue ;
		it->second->set_writebuf(RPL_PRIVMSG(client->get_nick(), client->get_user(), target, message));
	}
}

// sends a private message to a channel or client
// PRIVMSG #channel :hello OR PRIVMSG user :hello
void Server::command_privmsg(t_cmd cmd)
{
	if (cmd.message.find(":") == std::string::npos)
	{
		cmd.client->set_writebuf(ERR_NOTEXTTOSEND(cmd.client->get_nick()));
		return ;
	}

	std::string target = get_target(cmd.message);
	std::string message = get_message(cmd.message);

	if (message.empty())
	{
		cmd.client->set_writebuf(ERR_NOTEXTTOSEND(cmd.client->get_nick()));
		return ;
	}

	if (target.empty())
	{
		cmd.client->set_writebuf(ERR_NORECIPIENT(cmd.client->get_nick()));
		return ;
	}
	if (target[0] == '#')
	{
		Channel *channel = get_channel(target.substr(1));

		if (channel == NULL)
		{
			cmd.client->set_writebuf(ERR_NOSUCHNICK(cmd.client->get_nick(), target));
			return ;
		}
		broadcast_channel_message(channel, cmd.client, target, message);
	}
	else
	{
		Client *client = get_client_by_nick(target);

		if (client == NULL)
		{
			cmd.client->set_writebuf(ERR_NOSUCHNICK(cmd.client->get_nick(), target));
			return ;
		}
		client->set_writebuf(RPL_PRIVMSG(cmd.client->get_nick(), cmd.client->get_user(), target, message));
	}
}