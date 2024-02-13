#include "irc.hpp"

// generate a string of channel information: name, number of clients and topic
// LIST #test OR LIST
static std::string	get_list(std::string client_nick, Channel *channel)
{
	std::stringstream concat;
	
	concat << "322 " << client_nick << " #" << channel->get_name() << " "  \
	<< channel->get_client_map().size() << " "\
	<< (channel->get_topic().empty() ? ":No topic set for this channel yet."  : channel->get_topic()) \
	<< "\r\n";
	return (concat.str());			
}

void Server::command_list(t_cmd cmd)
{
	std::string nick = cmd.client->get_nick();
	std::string	RPL_LISTSTART		= "321 " + nick + " Channel :Users Name\r\n";
	std::string	RPL_LIST;
	std::string	RPL_LISTEND 		= "323 " + nick + " :End of /LIST\r\n";
	std::string channel_name;

	// extract channel name
	channel_name.clear();
	if (cmd.message.find('#') != std::string::npos)
		channel_name = get_channel_name(cmd.message.substr(cmd.message.find('#')));

	// if not channel name is provided list all channels
	if (channel_name.empty())
	{
		if (_channel_map.empty())
		{
			cmd.client->set_writebuf(RPL_LISTEND);
			return ;
		}
		cmd.client->set_writebuf(RPL_LISTSTART);
		std::map<std::string, Channel>::iterator it;
		for (it = _channel_map.begin(); it != _channel_map.end(); it++)
		{
			RPL_LIST.clear();
			RPL_LIST = get_list(nick, &it->second);
			cmd.client->set_writebuf(RPL_LIST);
		}
		cmd.client->set_writebuf(RPL_LISTEND);
	}
	else // list channel provided
	{
		Channel *channel = get_channel(channel_name);

		if (channel == NULL)
		{
			cmd.client->set_writebuf(RPL_LISTEND);
		}
		else
		{
			RPL_LIST = get_list(nick, channel);
			cmd.client->set_writebuf(RPL_LIST);
			cmd.client->set_writebuf(RPL_LISTEND);
		}

	}

}