#include "irc.hpp"

static std::string	find_nick(std::string msg_to_parse)
{
	std::string nickname;
	nickname.clear();
	
	if (msg_to_parse.empty() == true)
		return (nickname);
	char *str = const_cast<char *>(msg_to_parse.data());
	nickname = strtok(str, " ");
	
	if (nickname.empty() || nickname.find("#") != nickname.npos)		
		nickname.clear();
	return (nickname);
}

static std::string	find_channel(std::string msg_to_parse)
{
	std::string channel;

	if (msg_to_parse.empty() || msg_to_parse.find("#") == msg_to_parse.npos)
		channel.clear();
	else
		channel.append(msg_to_parse, msg_to_parse.find("#") + 1, std::string::npos);
	return (channel);
}

void Server::command_invite(t_cmd cmd)
{
	std::string nick = cmd.client->get_nick();
	std::string channel_name = find_channel(cmd.message);
	std::string invited_client = find_nick(cmd.message);

	if (channel_name.empty() || invited_client.empty())
	{
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(nick, cmd.name));
		return ;
	}

	Channel *channel = get_channel(channel_name);
	if (channel == NULL)
	{
		cmd.client->set_writebuf(ERR_NOSUCHCHANNEL(nick, channel_name));
		return ;
	}
	if (channel->get_client_by_name(nick) == NULL)
	{
		cmd.client->set_writebuf(ERR_NOTONCHANNEL(nick, channel_name));
		return ;
	}
	if (channel->get_client_by_name(invited_client) != NULL)
	{
		cmd.client->set_writebuf(ERR_USERONCHANNEL(nick, invited_client, channel_name));
		return ;
	}
	Client *inv_client = get_client_by_nick(invited_client);
	if (inv_client == NULL)
	{
		cmd.client->set_writebuf(ERR_NOSUCHNICK(nick, invited_client));
		return ;
	}
	channel->add_whitelist(inv_client);
	cmd.client->set_writebuf(RPL_INVITING(user_id(nick, cmd.client->get_user()), nick, invited_client, channel_name));
	inv_client->set_writebuf(RPL_INVITE(user_id(nick, cmd.client->get_user()), nick, channel_name));
}