# include "irc.hpp"

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