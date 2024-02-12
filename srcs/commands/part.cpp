#include "irc.hpp"

static bool is_valid_str(std::string str)
{
	if (str.size() < 2)
		return false;
	if (str[0] != '#')
		return false;
	if (str.find_first_of(" ,.\x07") != std::string::npos)
		return false;
	return true;
}

static std::vector<std::string> get_channels(std::string msg)
{
	std::vector<std::string>	channels;
	std::string tmp;
	channels.clear();
	size_t delim;

	msg = msg.substr(0, msg.find(" "));
	while (1)
	{
		delim = msg.find(",");
		tmp = msg.substr(0, delim);
		while (tmp.find(" ") != std::string::npos)
			tmp.erase(tmp.find(" "), 1);
		if (is_valid_str(tmp))
			channels.push_back(tmp.erase(0, 1));
		msg = msg.substr(delim + 1);
		if (delim == std::string::npos)
			break ;
	}
	return channels;
}

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
		channel->broadcast_string(RPL_PART(user_id(cmd.client->get_nick(), cmd.client->get_user()), *it, reason));
		channel->remove_client(cmd.client);
	}
}