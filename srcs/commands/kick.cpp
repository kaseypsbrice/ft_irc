#include "irc.hpp"

static std::string	get_kicked_name(std::string msg_to_parse)
{
	std::string kicked;

	if (msg_to_parse[0] == ' ')
		msg_to_parse.erase(0, 1);
	kicked = msg_to_parse.substr(msg_to_parse.find(' ') + 1, (msg_to_parse.find(':') - 1 - (msg_to_parse.find(' ') + 1)));
	return (kicked);

}

void Server::command_kick(t_cmd cmd)
{
	std::string nick = cmd.client->get_nick();
	std::string kicked_name = get_kicked_name(cmd.message);
	std::string channel_name = get_channel_name(cmd.message);
	std::string reason = get_reason(cmd.message);
	Channel *channel = get_channel(channel_name);

	reason = (reason.empty()) ? ":Kicked by the channel's operator" : reason;
	if (channel_name.empty() || kicked_name.empty())
	{
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(nick, cmd.name));
		return ;
	}
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
	if (channel->get_client_by_name(kicked_name) == NULL)
	{
		cmd.client->set_writebuf(ERR_USERONCHANNEL(nick, kicked_name, channel_name));
		return ;
	}
	if (channel->is_nick_operator(nick) == false)
	{
		cmd.client->set_writebuf(ERR_CHANOPRIVSNEEDED(nick, channel_name));
		return ;
	}
	channel->broadcast_string(RPL_KICK(user_id(nick, cmd.client->get_user()), channel_name, kicked_name, reason));
	channel->remove_client(channel->get_client_by_name(kicked_name));
}