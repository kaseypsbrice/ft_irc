#include "irc.hpp"

static std::string	get_name(std::string msg_to_parse)
{
	std::string name;

	name.clear();
	if (msg_to_parse.empty() == false || msg_to_parse.find(" ") != msg_to_parse.npos)
	{
		name.insert(0, msg_to_parse, 0,\
					msg_to_parse.find_last_of(" "));
	}
	return (name);
}

static std::string	get_password(std::string msg_to_parse)
{
	std::string password;

	password.clear();
	if (msg_to_parse.empty() == false || msg_to_parse.find(" ") != msg_to_parse.npos)
	{
		password.insert(0, msg_to_parse,\
					msg_to_parse.find_last_of(" ") + 1,\
					msg_to_parse.size() - 1);
	}
	return (password);
}

// "login" to operator account to gain operator rights
// clients do not persist after disconnection so this is poorly implemented
// operator accounts are registered manually with Server::add_operator()
// OPER user password
void Server::command_oper(t_cmd cmd)
{
	std::string name = get_name(cmd.message);
	std::string password = get_password(cmd.message);

	if (name.empty() || password.empty())
	{
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(cmd.client->get_nick(), cmd.name));
		return ;
	}
	std::map<std::string, std::string>::iterator it;
	it = _operator_map.find(name);
	if (it == _operator_map.end())
	{
		cmd.client->set_writebuf(ERR_NOOPERHOST(cmd.client->get_nick()));
		return ;
	}
	if (it->second != password)
	{
		cmd.client->set_writebuf(ERR_PASSWDMISMATCH(cmd.client->get_nick()));
		return ;
	}
	if (cmd.client->is_operator())
		return ;
	cmd.client->set_operator(true);
	cmd.client->set_writebuf(RPL_YOUREOPER(cmd.client->get_nick()));
}