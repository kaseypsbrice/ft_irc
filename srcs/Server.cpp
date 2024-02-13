#include "Server.hpp"

Server::Server(std::string port, std::string password) : _port(port), _password(password)
{}

Server::~Server()
{}

int Server::create_server()
{
	int	status;

	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_UNSPEC; // use either IPV4 or IPV6
	_hints.ai_flags = AI_PASSIVE; // socket will be used in bind ??
	_hints.ai_socktype = SOCK_STREAM; // create socket stream

	// gets addrinfo data structure for socket setup
	if ((status = getaddrinfo(NULL, _port.c_str(), &_hints, &_servinfo)) != 0){
		std::cerr << "getaddrinfo error : " << gai_strerror(status) << std::endl;
		return 1;
	}

	// get the file descriptor for the server socket
	_socket_fd = socket(_servinfo->ai_family, _servinfo->ai_socktype, _servinfo->ai_protocol);
	if (_socket_fd == -1){
		std::cerr << "Socket failed" << std::endl;
		return 1;
	}

	// set SO_REUSEADDR so the port can be resused immediately (late packets can be ignored when restarting server)
	int	value = 1;
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
	{
		std::cerr << "Setsockopt failed" << std::endl;
		return 1;
	}

	// binds the socket to port
	if (bind(_socket_fd, _servinfo->ai_addr, _servinfo->ai_addrlen) == -1)
	{
		std::cerr << "Bind failed" << std::endl;
		return 1;
	}

	// sets the socket to listen for connections
	if (listen(_socket_fd, 10) == -1)
	{
		std::cerr << "Listen failed" << std::endl;
		return 1;
	}
	freeaddrinfo(_servinfo);
	return 0;
}

int Server::accept_socket(int listenSocket) const
{
	sockaddr_in client;
	socklen_t addr_size = sizeof(sockaddr_in);
	return (accept(listenSocket, (sockaddr *)&client, &addr_size));
}

int Server::new_connection(std::vector<pollfd> &poll_fds, std::vector<pollfd> &new_poll_fds)
{
	(void)poll_fds; // todo: checking against max connections

	// accepts the connection, creating a dedicated file descriptor for communication with this client
	int	new_client_sock = accept_socket(_socket_fd);
	if (new_client_sock == -1)
	{
		std::cerr << "Accept failed" << std::endl;
		return 1;
	}

	// pollfd data structure stores file descriptor to recv/send data + flags
	pollfd new_client_poll;
	new_client_poll.fd = new_client_sock;
	new_client_poll.events = POLLIN | POLLOUT;

	// Client class to hold all the client's information
	Client new_client(new_client_poll.fd);
	if (_password == "")
		new_client.set_password_correct(true);
	_client_map.insert(std::pair<const int, Client>(new_client_poll.fd, new_client));
	new_poll_fds.push_back(new_client_poll);
	std::cout << "Client connection established on fd " << new_client_poll.fd << std::endl;
	std::cout << "Active connections " << poll_fds.size() << std::endl;
	return 0;
}

int	Server::server_loop()
{
	// stores all the file descriptors to poll (wait to send/recieve data)
	pollfd	server_poll_fd;

	server_poll_fd.fd = _socket_fd;
	server_poll_fd.events = POLLIN;

	_poll_fds.push_back(server_poll_fd);

	while (1)
	{
		std::vector<pollfd> new_poll_fds;

		if (poll((pollfd *)&_poll_fds[0], (unsigned int)_poll_fds.size(), -1) <= 0)
		{
			if (errno == EINTR) // interrupt signal to cleanly quit
				break ;
			std::cerr << "Poll error" << std::endl;
			return 1;
		}
		
		std::vector<pollfd>::iterator it = _poll_fds.begin();
		while (it != _poll_fds.end())
		{
			if (it->revents & POLLIN) // if the event is an input
			{
				if (it->fd == _socket_fd) // if the file descriptor is the server socket
				{
					// create a new connection, future data is sent to a separate client file descriptor
					if (new_connection(_poll_fds, new_poll_fds))
						continue ;
				}
				else
				{
					// if it is not the server file descriptor then it is an existing client
					if (handle_existing_client(_poll_fds, it))
						break ;
				}
			}
			else if (it->revents & POLLOUT)
			{
				// send client writebuf or close connection
				if (handle_poll_out(_poll_fds, it))
					break ;
			}
			else if (it->revents & POLLERR)
			{
				// exit with an error if poll on server fd fails, restart loop if client fails (client is removed)
				int	ret = handle_poll_err(it);
				if (ret == 1)
					break ;
				else if (ret == 2)
					return 1;
			}
			it++;
		}
		// add any new connections to the vector to poll
		_poll_fds.insert(_poll_fds.end(), new_poll_fds.begin(), new_poll_fds.end());
	}
	return 0;
}

void Server::add_client_to_channel(std::string name, Client *client)
{
	Channel *channel;

	channel = get_channel(name);
	if (!channel)
	{
		std::cout << "Could not find channel" << std::endl;
		return ;
	}
	channel->add_client(client);
}

// removes client from channels and server
void Server::remove_client(Client *client)
{
	std::map<std::string, Channel>::iterator channel_it;
	for (channel_it = _channel_map.begin(); channel_it != _channel_map.end(); channel_it++)
	{
		channel_it->second.remove_client(client);
	}

	std::vector<pollfd>::iterator poll_it;
	for (poll_it = _poll_fds.begin(); poll_it != _poll_fds.end(); poll_it++)
	{
		if (poll_it->fd == client->get_client_fd())
		{
			_poll_fds.erase(poll_it);
			break ;
		}
	}
	close(client->get_client_fd());
	_client_map.erase(client->get_client_fd());
	std::cout << "Client " << client->get_nick() << " removed on fd " << client->get_client_fd() << std::endl;
}

// looks up the client in the map by its file descriptor
Client *Server::get_client(const int client_fd)
{
	std::map<const int, Client>::iterator it_client = _client_map.find(client_fd);
	
	if (it_client == _client_map.end())
		return (NULL);
	return (&it_client->second);
}

// looks up the client by nickname
Client *Server::get_client_by_nick(std::string nick)
{
	std::map<const int, Client>::iterator it;

	for (it = _client_map.begin(); it != _client_map.end(); it++)
	{
		if (it->second.get_nick() == nick)
			return &it->second;
	}
	return NULL;
}

// looks up the channel in the map by its name
Channel* Server::get_channel(std::string name)
{
	std::map<std::string, Channel>::iterator it_channel = _channel_map.find(name);
	
	if (it_channel == _channel_map.end())
		return (NULL);
	return (&it_channel->second);
}

void Server::new_channel(std::string name)
{
	Channel new_channel(name);

	_channel_map.insert(std::pair<std::string, Channel>(name, new_channel));
}

void Server::add_operator(std::string user, std::string password)
{
	_operator_map.insert(std::pair<std::string, std::string>(user, password));
}

// send a message to all clients
void Server::broadcast_all(std::string message)
{
	std::map<const int, Client>::iterator it;

	for (it = _client_map.begin(); it != _client_map.end(); it++)
		it->second.set_writebuf(message);
}