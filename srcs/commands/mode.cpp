# include "irc.hpp"

// fills t_mode struct
// target = channel name
// mode = +i -i etc
// params = target of invite or operator mode
// initially contains all the modes provided and is duplicated to pass into
// each individual mode function
// "+o-l" mode is passed to mode_operator and mode_limit as "+o" and "-l" separately
// with the same target and same params
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

// +k -k sets or removes channel password
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

// +l -l sets or removes capacity of server
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

// +o doesnt edit mode string, makes a client operator of channel
void Server::mode_operator(t_mode mode, Client *client)
{
	Channel *channel = get_channel(mode.target);
	Client *new_op = get_client_by_nick(mode.params);

	if (mode.params.empty())
		return ;
	if (new_op == NULL)
	{
		client->set_writebuf(ERR_NOSUCHNICK(client->get_nick(), mode.params));
		client->set_writebuf(ERR_USERNOTINCHANNEL(client->get_nick(), mode.params, mode.target));
		return ;
	}
	if (channel->get_client_by_fd(new_op->get_client_fd()) == NULL)
	{
		client->set_writebuf(ERR_USERNOTINCHANNEL(client->get_nick(), mode.params, mode.target));
		return ;
	}
	if (mode.mode[0] == '+')
	{
		if (channel->is_nick_operator(mode.params))
			return ;
		
		channel->add_operator(new_op);
		channel->broadcast_string(MODE_CHANNELMSGWITHPARAM(mode.target, "+o", mode.params));
	}
	else
	{
		if (!channel->is_nick_operator(mode.params))
			return ;
		channel->remove_operator(new_op);
		channel->broadcast_string(MODE_CHANNELMSGWITHPARAM(mode.target, "-o", mode.params));
	}

}

// +i -i invite only mode
void Server::mode_invite(t_mode mode, Client *client)
{
	(void)client;
	Channel *channel = get_channel(mode.target);
	size_t pos = channel->get_mode().find("i");

	if (mode.mode[0] == '+')
	{
		if (pos != std::string::npos)
			return ;
		channel->add_mode("i");
		channel->broadcast_string(MODE_CHANNELMSG(mode.target, "+i"));
	}
	else
	{
		if (pos == std::string::npos)
			return ;
		channel->remove_mode("i");
		channel->broadcast_string(MODE_CHANNELMSG(mode.target, "-i"));
	}
}

// +t -t does topic command require operator
void Server::mode_topic(t_mode mode, Client *client)
{
	(void)client;
	Channel *channel = get_channel(mode.target);
	size_t pos = channel->get_mode().find("t");

	if (mode.mode[0] == '+')
	{
		if (pos != std::string::npos)
			return ;
		channel->add_mode("t");
		channel->broadcast_string(MODE_CHANNELMSG(mode.target, "+t"));
	}
	else
	{
		if (pos == std::string::npos)
			return ;
		channel->remove_mode("t");
		channel->broadcast_string(MODE_CHANNELMSG(mode.target, "-t"));
	}
}

// changes mode string of server: example mode string "+iot" 
// MODE #test +k+i pass (sets invite only mode and password to pass in #test channel)
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
			if (tmp.mode.find("k") != std::string::npos)
				mode_key(tmp, cmd.client);
			else if (tmp.mode.find("l") != std::string::npos)
				mode_limit(tmp, cmd.client);
			else if (tmp.mode.find("o") != std::string::npos)
				mode_operator(tmp, cmd.client);
			else if (tmp.mode.find("i") != std::string::npos)
				mode_invite(tmp, cmd.client);
			else if (tmp.mode.find("t") != std::string::npos)
				mode_topic(tmp, cmd.client);
			
		}
	}
}