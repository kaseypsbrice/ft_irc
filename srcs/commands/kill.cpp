#include "irc.hpp"

static std::string find_nick(std::string msg)
{
	while (msg[0] && msg[0] == ' ')
		msg.erase(0, 1);
	if (msg.empty())
		return std::string();
	return msg.substr(0, msg.find(' '));
}

// extract reason for kick or default to "no reason given"
static std::string find_comment(std::string msg)
{
	std::string comment;

	comment.clear();
	while (msg[0] && msg[0] == ' ')
		msg.erase(0, 1);
	if (msg.find(' ') == std::string::npos)
		return "no reason given";
	comment = msg.substr(msg.find(' ') + 1);
	while (comment[comment.size() - 1] == ' ')
		comment.erase(comment.size() - 1, 1);
	return comment.empty() ? "no reason given" : comment;
}

// terminates a clients connection, requires server operator
// KILL sam :too much spam (kills sam with the reason "too much spam")
void Server::command_kill(t_cmd cmd)
{
	std::string killed = find_nick(cmd.message);
	std::string killer = cmd.client->get_nick();
	std::string comment = find_comment(cmd.message);
	Client *killed_client = get_client_by_nick(killed);

	if (killed.empty())
	{
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(killer, cmd.name));
		return ;
	}
	if (killed_client == NULL)
	{
		cmd.client->set_writebuf(ERR_NOSUCHNICK(killer, killed));
		return ;
	}
	if (!cmd.client->is_operator())
	{
		cmd.client->set_writebuf(ERR_NOPRIVILEGES(killer));
		return ;
	}

	// inform killed client
	killed_client->set_writebuf(RPL_KILL(user_id(killer, cmd.client->get_user()), killed, comment));

	// inform the rest of the server
	std::string quit_reason = ":Killed (" + killer + " (" + comment + "))";
	broadcast_all(RPL_QUIT(user_id(killed, killed_client->get_user()), quit_reason));

	// inform client of connection termination
	std::string error_reason = ":Closing Link: localhost. Killed (" + killer + " (" + comment + "))";
	killed_client->set_writebuf(RPL_ERROR(user_id(killed, killed_client->get_user()), error_reason));

	// remove client
	killed_client->set_to_remove(true);
}