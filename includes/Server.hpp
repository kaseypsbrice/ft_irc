#ifndef SERVER_HPP
# define SERVER_HPP

# include <netdb.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <poll.h>
# include <iostream>
# include <vector>
# include <cerrno>
# include <map>
# include "Client.hpp"

class Server
{
private:
	int						_socket_fd;
	struct addrinfo			_hints;
	struct addrinfo			*_servinfo;
	std::string				_port;
	std::string				_password;
	std::map<const int, Client> _client_map;

	int accept_socket(int listenSocket) const;
	int new_connection(std::vector<pollfd> &poll_fds, std::vector<pollfd> &new_poll_fds);
	int	execute_command(std::string command);
	int handle_existing_client(std::vector<pollfd>& poll_fds, std::vector<pollfd>::iterator &it);
public:
	Server(std::string port, std::string password);
	~Server();
	int	create_server();
	int	server_loop();

	Client* get_client(const int client_fd);
};

#endif