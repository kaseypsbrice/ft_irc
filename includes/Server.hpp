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
# include <vector>
# include <fstream>
# include <unistd.h>
# include <algorithm>
# include <sstream>
# include "Client.hpp"
# include "Channel.hpp"
# include "replies.hpp"

typedef struct s_cmd 
{
	std::string	name;
	std::string	prefix;
	std::string	message;
	Client		*client;
}	t_cmd;

typedef struct s_mode
{
	std::string mode;
	std::string target;
	std::string params;
	std::string nick;
}	t_mode;

class Server
{
private:
	int						_socket_fd;
	struct addrinfo			_hints;
	struct addrinfo			*_servinfo;
	std::string				_port;
	std::string				_password;
	std::map<const int, Client> _client_map;
	std::map<std::string, Channel> _channel_map;
	std::vector<pollfd> _poll_fds;
	std::map<std::string, std::string> _operator_map;

	std::string	get_list_of_members(Channel *channel);
	int accept_socket(int listenSocket) const;
	int new_connection(std::vector<pollfd> &poll_fds, std::vector<pollfd> &new_poll_fds);
	int	execute_command(t_cmd cmd);
	int handle_existing_client(std::vector<pollfd>& poll_fds, std::vector<pollfd>::iterator &it);
	int handle_poll_out(std::vector<pollfd>& poll_fds, std::vector<pollfd>::iterator &it);
	int handle_poll_err(std::vector<pollfd>::iterator &it);
	int	process_message(Client *client);
	void command_nick(t_cmd cmd);
	void command_user(t_cmd cmd);
	void command_privmsg(t_cmd cmd);
	void command_join(t_cmd cmd);
	void command_pass(t_cmd cmd);
	void command_quit(t_cmd cmd);
	void command_mode(t_cmd cmd);
	void command_invite(t_cmd cmd);
	void command_ping(t_cmd cmd);
	void command_oper(t_cmd cmd);
	void command_part(t_cmd cmd);
	void mode_key(t_mode mode, Client *client);
	void mode_limit(t_mode mode, Client *client);
	void mode_operator(t_mode mode, Client *client);
	void mode_invite(t_mode mode, Client *client);
	void mode_topic(t_mode mode, Client *client);
	void send_reply(const int client_fd, std::string buf);
	void add_client_to_channel(std::string name, Client *client);
	void new_channel(std::string name);
	void broadcast_channel_join(Channel *channel, Client *client);
	void broadcast_channel_message(Channel *channel, Client *client, std::string target, std::string message);
	void remove_client(Client *client);
	bool is_nick_taken(std::string nick, const int client_fd);

public:
	Server(std::string port, std::string password);
	~Server();
	int	create_server();
	int	server_loop();

	Client *get_client(const int client_fd);
	Client *get_client_by_nick(std::string nick);
	Channel *get_channel(std::string name);
	void add_operator(std::string user, std::string password);
};

#endif