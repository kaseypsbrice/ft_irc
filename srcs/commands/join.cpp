# include "irc.hpp"

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

static std::vector<std::string> get_keys(std::string msg)
{
	std::vector<std::string>	keys;
	std::string tmp;
	keys.clear();
	size_t delim;

	delim = msg.find(" ");
	if (delim == std::string::npos || msg.size() < delim + 2)
		return keys;
	msg = msg.substr(delim + 1);
	while (1)
	{
		delim = msg.find(",");
		tmp = msg.substr(0, delim);
		while (tmp.find(" ") != std::string::npos)
			tmp.erase(tmp.find(" "), 1);
		if (is_valid_str(tmp))
			keys.push_back(tmp.erase(0, 1));
		msg = msg.substr(delim + 1);
		if (delim == std::string::npos)
			break ;
	}
	return keys;
}

void Server::broadcast_channel_join(Channel *channel, Client *client)
{
	std::string nick = client->get_nick();
	std::string user = client->get_user();
	std::string channel_name = channel->get_name();

	std::map<int, Client *>::iterator it;

	for (it = channel->get_client_map().begin(); it != channel->get_client_map().end(); it++)
	{

		it->second->set_writebuf(RPL_JOIN(user_id(nick, user), channel_name));
		if (!channel->get_topic().empty())
			it->second->set_writebuf(RPL_TOPIC(nick, channel_name, channel->get_topic()));
		
		std::string	list_of_members = get_list_of_members(channel);
		std::string symbol = channel->get_symbol();

		it->second->set_writebuf(RPL_NAMREPLY(user, symbol, channel_name, list_of_members));
		it->second->set_writebuf(RPL_ENDOFNAMES(user, channel_name));
	}
}

std::string	Server::get_list_of_members(Channel *channel)
{
	std::map<const int, Client *> client_map = channel->get_client_map();
	std::map<const int, Client *>::iterator	it;
	std::string nick;
	std::string members_list;

	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		nick.clear();
		nick = it->second->get_nick();
			
		if (channel->is_nick_operator(nick))
			members_list += "@";
		members_list += nick;
		members_list += " ";
	}
	if (members_list.size() >= 1 && members_list[members_list.size() - 1] == ' ')
		members_list.erase(members_list.end()-1);
	return (members_list);
}

void Server::command_join(t_cmd cmd)
{
	//cmd.message.erase(std::remove(cmd.message.begin(), cmd.message.end(), '\n'), cmd.message.end());

	std::vector<std::string> channels = get_channels(cmd.message);
	std::vector<std::string> keys = get_keys(cmd.message);

	if (channels.empty())
	{
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(cmd.client->get_nick(), cmd.name));
		return ;
	}

	std::vector<std::string>::iterator channels_it = channels.begin();
	std::vector<std::string>::iterator keys_it = keys.begin();

	while (channels_it != channels.end())
	{
		// search for existing channel
		std::map<std::string, Channel>::iterator existing_channel = _channel_map.find(*channels_it);
		if (existing_channel == _channel_map.end())
		{
			// if channel does not exist, create it
			new_channel(*channels_it);
		}
		else
		{
			// check if key does not match channel password
			if (!existing_channel->second.get_channel_password().empty() && \
			(keys_it == keys.end() || *keys_it != existing_channel->second.get_channel_password()))
			{
				cmd.client->set_writebuf(ERR_BADCHANNELKEY(cmd.client->get_nick(), existing_channel->second.get_name()));
				channels_it++;
				if (keys_it != keys.end())
					keys_it++;
				continue;
			}
		}

		// retrieve the channel
		Channel *current_channel = get_channel(*channels_it);
		// check if channel is full
		if (current_channel->is_full())
		{
			cmd.client->set_writebuf(ERR_CHANNELISFULL(cmd.client->get_nick(), existing_channel->second.get_name()));
			channels_it++;
			if (keys_it != keys.end())
				keys_it++;
			continue;
		}
		// todo check if banned or invite only

		add_client_to_channel(*channels_it, cmd.client);
		

		// if the channel has no operators (new channel) add the client
		if (current_channel->get_operators().empty())
			current_channel->add_operator(cmd.client);
		// broadcast to channel members
		broadcast_channel_join(current_channel, cmd.client);

		channels_it++;
		if (keys_it != keys.end())
			keys_it++;
	}
}