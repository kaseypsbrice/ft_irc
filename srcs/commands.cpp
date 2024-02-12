# include "irc.hpp"

// stolen function to process commands (we dont need to do all of these I think)
int	Server::execute_command(t_cmd cmd)
{
	std::string	valid_cmds[VALID_COMMANDS] = {
		"INVITE", //
		"JOIN",
		"KICK", //
		"KILL",
		"LIST",
		"MODE",
		"MOTD",
		"NAMES",
		"NICK",
		"PASS",
		"OPER", //
		"PART", //
		"PING",
		"PRIVMSG",
		"QUIT",
		"TOPIC", //
		"USER",
		};

	//Client *client = getClient(this, client_fd);
	//cmd_struct cmd_infos;
	int index = 0;

	//if (parseCommand(cmd_line, cmd_infos) == FAILURE)
	//	return ;

	while (index < VALID_COMMANDS)
	{
		if (cmd.name == valid_cmds[index])
			break;
		index++;
	}

	switch (index + 1)
	{
		case 1: command_invite(cmd); break;
		case 2: command_join(cmd); break;
		case 3: ; break; // kick
		case 4: ; break;
		case 5: ; break;
		case 6: command_mode(cmd); break;
		case 7: ; break;
		case 8: ; break;
		case 9: command_nick(cmd); break;
    	case 10: command_pass(cmd); break;
		case 11: command_oper(cmd); break;
		case 12: ; break; // part
		case 13: command_ping(cmd); break;
		case 14: command_privmsg(cmd); break;
		case 15: command_quit(cmd); break;
		case 16: ; break; // topic
		case 17: command_user(cmd); break;
		default:
			std::cout << "Command not found: " << cmd.name << std::endl;
			cmd.client->set_writebuf(ERR_UNKNOWNCOMMAND(cmd.client->get_nick(), cmd.name));
	}
	return 0;
}

