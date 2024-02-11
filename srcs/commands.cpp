# include "irc.hpp"

// stolen function to process commands (we dont need to do all of these I think)
int	Server::execute_command(t_cmd cmd)
{
	std::string	valid_cmds[VALID_COMMANDS] = {
		"INVITE",
		"JOIN",
		"KICK",
		"KILL",
		"LIST",
		"MODE",
		"MOTD",
		"NAMES",
		"NICK",
		"NOTICE",
		"OPER",
		"PART",
		"PING",
		"PRIVMSG",
		"QUIT",
		"TOPIC",
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
		case 1: ; break;
		case 2: command_join(cmd); break;
		case 3: ; break;
		case 4: ; break;
		case 5: ; break;
		case 6: ; break;
		case 7: ; break;
		case 8: ; break;
		case 9: command_nick(cmd); break;
    	case 10: ; break;
		case 11: ; break;
		case 12: ; break;
		case 13: ; break;
		case 14: command_privmsg(cmd); break;
		case 15: ; break;
		case 16: ; break;
		case 17: command_user(cmd); break;
		default:
			std::cout << "Command not found: " << cmd.name << std::endl;
			//addToClientBuffer(this, client_fd, ERR_UNKNOWNCOMMAND(client->getNickname(), cmd_infos.name));
	}
	return 0;
}

