# include "irc.hpp"

int Server::process_message(Client *client)
{
	int			line_break;
	std::string line;
	std::string &readbuf = client->get_readbuf();
	
	// IRC commands are terminated by "\r\n"
	while (readbuf.find("\r\n") != std::string::npos)
	{
		t_cmd	cmd;
		cmd.client = client;
		line_break = readbuf.find("\r\n");
		line = readbuf.substr(0, line_break);
		
		// ':' at start indicates prefix :localhost... etc
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

		execute_command(cmd);

		readbuf = readbuf.substr(line_break + 2);
	}
	return (0);
}

void Server::send_reply(const int client_fd, std::string buf)
{
	if (buf.size() > 0)
	{
		std::cout << "[SEND][" << get_client(client_fd)->get_nick() << "] " << buf;
		send(client_fd, buf.c_str(), buf.size(), 0);
	}
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
	(void)poll_fds;
	if (client->is_to_remove())
	{
		remove_client(client);
		return 1;
	}
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
		client->set_to_remove(true);
		return 1;
	}
	if (size == 0)
	{
		std::cout << "Empty message" << std::endl;
		remove_client(client);
		return 1;
	}
	std::cout << std::endl << "[RECIEVE][" << get_client(it->fd)->get_nick() << "] " << message << std::endl;
	// messages are buffered in case of partial data transfer
	client->set_readbuf(message);
	process_message(client);
	return 0;
}

int Server::handle_poll_err(std::vector<pollfd>::iterator &it)
{
	std::cout << "Poll error" << std::endl;
	if (it->fd == _socket_fd)
	{
		std::cout << "Listen socket error" << std::endl;
		return 2;
	}
	else
	{
		Client *client = get_client(it->fd);
		if (client == NULL)
		{
			std::cout << "handle_poll_err: unable to find client" << std::endl;
			return 1;
		}
		client->set_to_remove(true);
		return 1;
	}
	return 0;
}