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
	_client_map.insert(std::pair<const int, Client>(new_client_poll.fd, new_client));
	new_poll_fds.push_back(new_client_poll);
	std::cout << "Client connection established" << std::endl;
	return 0;
}

int	Server::server_loop()
{
	std::vector<pollfd> poll_fds; // stores all the file descriptors to poll (wait to send/recieve data)
	pollfd	server_poll_fd;

	server_poll_fd.fd = _socket_fd;
	server_poll_fd.events = POLLIN;

	poll_fds.push_back(server_poll_fd);

	while (1)
	{
		std::vector<pollfd> new_poll_fds;

		//std::cout << "polling..." << std::endl;
		if (poll((pollfd *)&poll_fds[0], (unsigned int)poll_fds.size(), -1) <= 0)
		{
			if (errno == EINTR) // interrupt signal to cleanly quit
				break ;
			std::cerr << "Poll error" << std::endl;
			return 1;
		}
		
		std::vector<pollfd>::iterator it = poll_fds.begin();
		while (it != poll_fds.end())
		{
			if (it->revents & POLLIN) // if the event is an input
			{
				if (it->fd == _socket_fd) // if the file descriptor is the server socket
				{
					// create a new connection, future data is sent to a separate client file descriptor
					if (new_connection(poll_fds, new_poll_fds))
						continue ;
				}
				else
				{
					// if it is not the server file descriptor then it is an existing client
					handle_existing_client(poll_fds, it);
				}
			}
			else if (it->revents & POLLOUT)
			{
				handle_poll_out(poll_fds, it);
			}
			it++;
		}
		// add any new connections to the vector to poll
		poll_fds.insert(poll_fds.end(), new_poll_fds.begin(), new_poll_fds.end());
	}
	return 0;
}

// looks up the client in the map by its file descriptor
Client* Server::get_client(const int client_fd)
{
	std::map<const int, Client>::iterator it_client = _client_map.find(client_fd);
	
	if (it_client == _client_map.end())
		return (NULL);
	return (&it_client->second);
}