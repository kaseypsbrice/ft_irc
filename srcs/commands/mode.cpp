# include "irc.hpp"

static void	fill_mode(t_mode &mode, std::string command)
{	
	size_t	pos;

	pos = command.find(" ");
	if (pos == std::string::npos)
	{
		mode.target = command.substr(0);
		return ;
	}
	else
	{
		mode.target = command.substr(0, pos);
		command.erase(0, pos + 1);
	}

	pos = command.find(" ");
	if (pos == std::string::npos)
	{
		mode.mode = command.substr(0);
		return ;
	}
	else
	{
		mode.mode = command.substr(0, pos);
		command.erase(0, pos + 1);
	}

	mode.params = command.substr(0);
}

void Server::mode_key(t_mode mode, Client *client)
{
	Channel *channel = get_channel(mode.target);

	size_t pos = channel->get_mode().find("k");
	if (mode.params.empty())
		return ;
	if (mode.mode[0] == '+')
	{
		if (pos != std::string::npos)
			return ;
		if (!is_alpha(mode.params))
		{
			client->set_writebuf(ERR_INVALIDMODEPARAM(client->get_nick(), mode.target, "+k", mode.params));
			return ;
		}
		channel->add_mode("k");
		channel->set_channel_password(mode.params);
		channel->broadcast_string(MODE_CHANNELMSGWITHPARAM(mode.target, "+k", mode.params));
	}
	else
	{
		if (pos == std::string::npos)
			return ;
		channel->remove_mode("k");
		channel->remove_channel_password();
		channel->broadcast_string(MODE_CHANNELMSGWITHPARAM(mode.target, "-k", mode.params));
	}
}

void Server::mode_limit(t_mode mode, Client *client)
{
	Channel *channel = get_channel(mode.target);
	(void)client;

	size_t pos = channel->get_mode().find("l");
	int	limit = atoi(mode.params.c_str());
	if (mode.mode[0] == '+')
	{
		if (limit < 0 || (int)channel->get_client_map().size() > limit)
			return ;
		channel->set_capacity(limit);
		if (pos != std::string::npos)
			return ;
		channel->add_mode("l");
		std::ostringstream ss; 
        ss << limit;
		channel->broadcast_string(MODE_CHANNELMSGWITHPARAM(mode.target, "+l", ss.str()));
	}
	else if (mode.mode[0] == '-')
	{
		channel->set_capacity(-1);
		if (pos == std::string::npos)
			return ;
		channel->remove_mode("l");
		channel->broadcast_string(MODE_CHANNELMSG(mode.target, "-l"));
	}
}

void Server::command_mode(t_cmd cmd)
{
	t_mode mode;

	mode.nick = cmd.client->get_nick();
	fill_mode(mode, cmd.message);
	if (mode.target.empty() || mode.target[0] != '#')
	{
		cmd.client->set_writebuf(ERR_NOSUCHCHANNEL(cmd.client->get_nick(), mode.target));
		return ;
	}
	mode.target.erase(0, 1);
	Channel *channel = get_channel(mode.target);
	if (channel == NULL)
	{
		cmd.client->set_writebuf(ERR_NOSUCHCHANNEL(cmd.client->get_nick(), mode.target));
		return ;
	}
	if (mode.mode.empty())
	{
		if (channel->is_nick_operator(cmd.client->get_nick()) && !channel->get_channel_password().empty())
			cmd.client->set_writebuf(RPL_CHANNELMODEISWITHKEY(cmd.client->get_nick(), mode.target, channel->get_mode(), channel->get_channel_password()));
		else
			cmd.client->set_writebuf(RPL_CHANNELMODEIS(cmd.client->get_nick(), mode.target, channel->get_mode()));
		return ;
	}
	if (!channel->is_nick_operator(cmd.client->get_nick()))
	{
		cmd.client->set_writebuf(ERR_CHANOPRIVSNEEDED(cmd.client->get_nick(), channel->get_name()));
		return ;
	}
	std::cout << "mode: " << mode.mode << std::endl;
	std::cout << "target: " << mode.target << std::endl;
	std::cout << "params: " << mode.params << std::endl;
	if (mode.mode[0] != '+' && mode.mode[0] != '-')
		return ;

	for (int i = 0; mode.mode[i]; i++)
	{
		if ((mode.mode[i] == '+' || mode.mode[i] == '-') && mode.mode[i + 1])
		{
			t_mode tmp = mode;
			tmp.mode = mode.mode.substr(0, i + 2);
			mode.mode.erase(0, i + 2);
			i = 0;
			std::cout << tmp.mode << std::endl;
			if (tmp.mode.find("k") != std::string::npos)
				mode_key(tmp, cmd.client);
			else if (tmp.mode.find("l") != std::string::npos)
				mode_limit(tmp, cmd.client);
		}
	}
}