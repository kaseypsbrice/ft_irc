#include "irc.hpp"

static std::string get_user_name(std::string msg)
{
	std::string user_name;
	user_name.clear();

	if (msg.empty() == false)
	{
		if (msg.find_first_of(' ') != std::string::npos)
			user_name.insert(0, msg, 0, msg.find(' '));
		else
			user_name.insert(0, msg, 0, std::string::npos);
	}
	return (user_name);
}

static std::string get_real_name(std::string msg)
{
	std::string real_name;
	real_name.clear();

	if (msg.empty())
		return (real_name);
	
	int space_count = 0;
	int	begin_pos = 0;
	for (size_t i = 0; i < msg.size(); i++)
	{
		if (msg[i] == ' ')
			space_count++;
		if (space_count == 3)
		{
			begin_pos = i;
			break;
		}
	}
	if (space_count >= 3)
		real_name.insert(0, msg, begin_pos + 1, std::string::npos); 
	if (real_name[0] == ':')
		real_name.erase(0, 1);
	return (real_name);
}

// sets the username and realname of client
// second and third arguments are discarded
// USER sam 0 * samuel
void Server::command_user(t_cmd cmd)
{
	std::string user_name = get_user_name(cmd.message);
	std::string real_name = get_real_name(cmd.message);

	std::cout << "user: " << user_name << "real: " << real_name << std::endl;

	if (user_name.empty() || real_name.empty())
		cmd.client->set_writebuf(ERR_NEEDMOREPARAMS(cmd.client->get_nick(), cmd.message));
	else if (cmd.client->is_registered())
		cmd.client->set_writebuf(ERR_ALREADYREGISTERED(cmd.client->get_nick()));
	else
	{
		cmd.client->set_user(user_name);
		cmd.client->set_real(real_name);
		cmd.client->set_user_registered(true);
		if (!cmd.client->is_registered() && cmd.client->is_nick_registered())
		{
			// check if server password is correct
			if (!cmd.client->is_password_correct())
			{
				cmd.client->set_writebuf(ERR_PASSWDMISMATCH(cmd.client->get_nick()));
				cmd.client->set_to_remove(true);
				return ;
			}

			// if not registered and nickname has been set, welcome client to server
			cmd.client->set_registered(true);
			cmd.client->set_writebuf(RPL_WELCOME(user_id(cmd.client->get_nick(), cmd.client->get_user()), cmd.client->get_nick()));
		}
	}
}