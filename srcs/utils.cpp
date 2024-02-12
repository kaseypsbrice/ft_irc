# include "irc.hpp"

std::string	get_reason(std::string msg)
{
	std::string reason;
	
	reason.clear();
	
	if (msg.find(":") != msg.npos)
		reason.append(msg, msg.find(":") + 1, std::string::npos);
	return (reason);
}
