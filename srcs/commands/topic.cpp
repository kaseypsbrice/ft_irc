#include "irc.hpp"

static std::string	find_channel_name(std::string msg_to_parse)
{
	std::string channel_name;
	channel_name.clear();
	
	if (msg_to_parse.empty() || msg_to_parse.find("#") == msg_to_parse.npos) // Si pas d'arg ou pas de chan (#)
		return (channel_name);
	else if (msg_to_parse.find(":") != msg_to_parse.npos) // Avec irssi, meme avec un mot, le client rajoute un ':'
	{
		char *str = const_cast<char *>(msg_to_parse.data());
		channel_name = strtok(str, " ");
		channel_name.erase(channel_name.find("#"), 1);
	}
	else
	{
		size_t i = 0;
		while (msg_to_parse[i] && (!isalpha(msg_to_parse[i]) && !isdigit(msg_to_parse[i]) && msg_to_parse[i] != '-' && msg_to_parse[i] != '_'))
			i++;
		while (msg_to_parse[i] && (isalpha(msg_to_parse[i]) || msg_to_parse[i] == '-' || msg_to_parse[i] == '_' || isdigit(msg_to_parse[i])))
			channel_name += msg_to_parse[i++];
	}
	return (channel_name);
}

static std::string	find_topic(std::string msg_to_parse)
{
	std::string topic;
	

	if (msg_to_parse.empty() || msg_to_parse.find(":") == msg_to_parse.npos)
		topic.clear();
	else
	{
		topic.append(msg_to_parse, msg_to_parse.find(":"), std::string::npos);
	}
	return (topic);
}

// sets or prints topic of channel, setting requires channel operator if +t mode is set
// TOPIC #channel OR TOPIC #channel :new topic
void Server::command_topic(t_cmd cmd)
{
	std::string nick = cmd.client->get_nick();
	std::string channel_name = find_channel_name(cmd.message);

	if (channel_name.empty())
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
	std::string topic = find_topic(cmd.message);
	if (topic.empty())
	{
		if (!channel->get_topic().empty())
			cmd.client->set_writebuf(RPL_TOPIC(nick, channel_name, channel->get_topic()));
		else
			cmd.client->set_writebuf(RPL_NOTOPIC(nick, channel_name));
	}
	else
	{
		if (channel->get_mode().find('t') != std::string::npos \
		&& !channel->is_nick_operator(nick))
		{
			cmd.client->set_writebuf(ERR_CHANOPRIVSNEEDED(nick, channel_name));
		}
		else
		{
			if (topic == ":")
				topic.clear();
			channel->set_topic(topic);
			channel->broadcast_string(RPL_TOPIC(nick, channel_name, topic));
		}
	}
}