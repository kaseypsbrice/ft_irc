#include "irc.hpp"

// replies to PING message with PONG message
// PING message
void Server::command_ping(t_cmd cmd)
{
	cmd.message.insert(0, ":");
	cmd.client->set_writebuf(RPL_PONG(user_id(cmd.client->get_nick(), cmd.client->get_user()), cmd.message));
}