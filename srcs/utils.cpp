# include "irc.hpp"

// returns a substring after ':' "KICK #channel sam :stinky" = "stinky"
std::string	get_reason(std::string msg)
{
	std::string reason;
	
	reason.clear();
	
	if (msg.find(":") != msg.npos)
		reason.append(msg, msg.find(":") + 1, std::string::npos);
	return (reason);
}

bool is_alpha(std::string str) 
{
    for (size_t i = 0; i < str.length(); i++) 
	{
        if (!isalpha(str[i])) 
            return (false);
    }
    return (true);
}

// returns a channel name or empty string if no # is found " #test " = "test"
std::string	get_channel_name(std::string msg_to_parse)
{
	std::string channel_name;
	size_t i = 0;
	//while (msg_to_parse[i] && (!isalpha(msg_to_parse[i]) && !isdigit(msg_to_parse[i]) && msg_to_parse[i] != '-' && msg_to_parse[i] != '_'))
	//	i++;
	while (msg_to_parse[i] && msg_to_parse[i] != '#')
		i++;
	if (msg_to_parse[i] != '#')
		return channel_name;
	i++;
	while (msg_to_parse[i] && (isalpha(msg_to_parse[i]) || msg_to_parse[i] == '-' || msg_to_parse[i] == '_' || isdigit(msg_to_parse[i])))
		channel_name += msg_to_parse[i++];
	return (channel_name);
}

// returns a list of channel names with the # removed "#test,#test2" = ["test1", "test2"]
std::vector<std::string> get_channels(std::string msg)
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
		tmp = get_channel_name(tmp);
		if (!tmp.empty())
			channels.push_back(tmp);
		if (delim == std::string::npos)
			break ;
		msg = msg.substr(delim + 1);
	}
	return channels;
}