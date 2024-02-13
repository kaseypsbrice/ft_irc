#include "irc.hpp"

// must be passed before registering NICK and USER
// PASS pass
void Server::command_pass(t_cmd cmd)
{
	if (cmd.client->is_registered())
	{
		cmd.client->set_writebuf(ERR_ALREADYREGISTERED(cmd.client->get_nick()));
		return ;
	}
	else if (cmd.message.empty())
	{
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(cmd.client->get_nick(), cmd.name));
		return ;
	}
	else if (_password != cmd.message)
	{
		cmd.client->set_writebuf(ERR_PASSWDMISMATCH(cmd.client->get_nick()));
		return ;
	}
	cmd.client->set_password_correct(true);
}