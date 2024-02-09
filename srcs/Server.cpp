#include "Server.hpp"

Server::Server(std::string port, std::string password) : _port(port), _password(password)
{}

Server::~Server()
{}

int Server::create_server()
{
	int	status;

	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_UNSPEC;
	_hints.ai_flags = AI_PASSIVE;
	_hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(NULL, _port.c_str(), &_hints, &_servinfo)) != 0){
		std::cerr << "getaddrinfo error : " << gai_strerror(status) << std::endl;
		return 1;
	}
	_socket_fd = socket(_servinfo->ai_family, _servinfo->ai_socktype, _servinfo->ai_protocol);
	if (_socket_fd == -1){
		std::cerr << "Socket failed" << std::endl;
		return 1;
	}
	int	value = 1;
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
	{
		std::cerr << "Setsockopt failed" << std::endl;
		return 1;
	}
	if (bind(_socket_fd, _servinfo->ai_addr, _servinfo->ai_addrlen) == -1)
	{
		std::cerr << "Bind failed" << std::endl;
		return 1;
	}
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

int	Server::server_loop()
{
	std::vector<pollfd> poll_fds;
	pollfd	server_poll_fd;

	server_poll_fd.fd = _socket_fd;
	server_poll_fd.events = POLLIN;

	poll_fds.push_back(server_poll_fd);

	while (1)
	{
		std::vector<pollfd> new_pollfds;

		std::cout << "polling..." << std::endl;
		if (poll((pollfd *)&poll_fds[0], (unsigned int)poll_fds.size(), -1) <= 0)
		{
			if (errno == EINTR)
				break ;
			std::cerr << "Poll error" << std::endl;
			return 1;
		}
		
		std::vector<pollfd>::iterator it = poll_fds.begin();
		while (it != poll_fds.end())
		{
			if (it->revents & POLLIN)
			{
				if (it->fd == _socket_fd)
				{
					int	new_client_sock = accept_socket(_socket_fd);
					if (new_client_sock == -1)
					{
						std::cerr << "Accept failed" << std::endl;
					}
					else
					{
						pollfd new_client_poll;
						new_client_poll.fd = new_client_sock;
						new_client_poll.events = POLLIN | POLLOUT;
						poll_fds.push_back(new_client_poll);
						std::cout << "Client connection established" << std::endl;
					}
				}
			}
			it++;
		}

	}
	return 0;
}