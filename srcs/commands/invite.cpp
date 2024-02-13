#include "irc.hpp"

// extract client nickname to invite
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

// extract channel to invite to
static std::string	find_channel(std::string msg_to_parse)
{
	std::string channel;

	if (msg_to_parse.empty() || msg_to_parse.find("#") == msg_to_parse.npos)
		channel.clear();
	else
		channel.append(msg_to_parse, msg_to_parse.find("#") + 1, std::string::npos);
	return (channel);
}

// invite client to channel
// INVITE #test sam (invite same to #test)
void Server::command_invite(t_cmd cmd)
{
	std::string nick = cmd.client->get_nick();
	std::string channel_name = find_channel(cmd.message);
	std::string invited_client = find_nick(cmd.message);

	if (channel_name.empty() || invited_client.empty())
	{
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(nick, cmd.name));
		return ; // client or channel not provided/invalid
	}
	Channel *channel = get_channel(channel_name);
	if (channel == NULL)
	{
		cmd.client->set_writebuf(ERR_NOSUCHCHANNEL(nick, channel_name));
		return ; // channel not found
	}
	if (channel->get_client_by_name(nick) == NULL)
	{
		cmd.client->set_writebuf(ERR_NOTONCHANNEL(nick, channel_name));
		return ; // client is not in channel
	}
	if (channel->get_client_by_name(invited_client) != NULL)
	{
		cmd.client->set_writebuf(ERR_USERONCHANNEL(nick, invited_client, channel_name));
		return ; // invited client already in channel
	}
	Client *inv_client = get_client_by_nick(invited_client);
	if (inv_client == NULL)
	{
		cmd.client->set_writebuf(ERR_NOSUCHNICK(nick, invited_client));
		return ; // client does not exist
	}
	channel->add_whitelist(inv_client); // add client to whitelist so they can join invite only server
	cmd.client->set_writebuf(RPL_INVITING(user_id(nick, cmd.client->get_user()), nick, invited_client, channel_name));
	inv_client->set_writebuf(RPL_INVITE(user_id(nick, cmd.client->get_user()), nick, channel_name));
}