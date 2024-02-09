# include "irc.hpp"

// stolen function to process commands
int	Server::execute_command(std::string command)
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
		if (command == valid_cmds[index])
			break;
		index++;
	}

	switch (index + 1)
	{
		case 1: ; break;
		case 2: ; break;
		case 3: ; break;
		case 4: ; break;
		case 5: ; break;
		case 6: ; break;
		case 7: ; break;
		case 8: ; break;
		case 9: ; break;
    	case 10: ; break;
		case 11: ; break;
		case 12: ; break;
		case 13: ; break;
		case 14: ; break;
		case 15: ; break;
		case 16: ; break;
		case 17: ; break;
		default:
			std::cout << "Command not found: " << command << std::endl;
			//addToClientBuffer(this, client_fd, ERR_UNKNOWNCOMMAND(client->getNickname(), cmd_infos.name));
	}
	return 0;
}