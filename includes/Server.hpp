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

class Server
{
private:
	int						_socket_fd;
	struct addrinfo			_hints;
	struct addrinfo			*_servinfo;
	std::string				_port;
	std::string				_password;

	int accept_socket(int listenSocket) const;
public:
	Server(std::string port, std::string password);
	~Server();
	int	create_server();
	int	server_loop();
};

#endif