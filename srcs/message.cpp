# include "irc.hpp"

int Server::process_message(Client *client)
{
	int			line_break;
	std::string line;
	std::string &readbuf = client->get_readbuf();
	
	// IRC commands are terminated by "\r\n"
	while (readbuf.find("\r\n") != std::string::npos)
	{
		//printf("size: %ld\n", readbuf.size());
		t_cmd	cmd;
		cmd.client = client;
		line_break = readbuf.find("\r\n");
		line = readbuf.substr(0, line_break);
		
		if (line[0] == ':')
		{
			cmd.prefix = line.substr(0, line.find(" "));
			line = line.substr(line.find(" ") + 1);
		}
		cmd.name = line.substr(0, line.find(" "));
		if (line.find(" ") != std::string::npos)
		{
			line = line.substr(line.find(" ") + 1);
			cmd.message = line;
		}

		for (size_t i = 0; i < cmd.name.size(); i++)
			cmd.name[i] = std::toupper(cmd.name[i]);
		
		//printf("Command Name:    %s | %ld\n", cmd.name.c_str(), cmd.name.size());
		//printf("Command Prefix:  %s | %ld\n", cmd.prefix.c_str(), cmd.prefix.size());
		//printf("Command Message: %s | %ld\n", cmd.message.c_str(), cmd.message.size());

		execute_command(cmd);

		//printf("lb: %d size: %ld\n", line_break + 2, readbuf.size());
		readbuf = readbuf.substr(line_break + 2);
	}
	return (0);
}

void Server::send_reply(const int client_fd, std::string buf)
{
	send(client_fd, buf.c_str(), buf.size(), 0);
}

int Server::handle_poll_out(std::vector<pollfd>& poll_fds, std::vector<pollfd>::iterator &it)
{
	Client *client = get_client(it->fd);
	if (!client)
	{
		std::cout << "handle_poll_out: failed to find client" << std::endl;
		return 0;
	}
	send_reply(it->fd, client->get_writebuf());
	client->get_writebuf().clear();
	(void)poll_fds; // todo client disconnect
	return 0;
}

int	Server::handle_existing_client(std::vector<pollfd>& poll_fds, std::vector<pollfd>::iterator &it)
{
	Client	*client;
	client = get_client(it->fd); // retrieve client from map
	char message[MESSAGE_BUF]; // buffer for recv
	int size; // holds message size

	(void)poll_fds;
	memset(message, 0, sizeof(message));
	size = recv(it->fd, message, MESSAGE_BUF, 0);

	if (size < 0)
	{
		std::cerr << "Recv failed" << std::endl;
		return 1;
	}
	if (size == 0)
	{
		// client disconnected
		return 1;
	}
	std::cout << "Recieved Message:" << std::endl << message;
	// messages are buffered in case of partial data transfer
	client->set_readbuf(message);
	process_message(client);
	return 0;
}