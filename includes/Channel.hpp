#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include <map>
# include <vector>
# include <iostream>
# include <algorithm>

class Channel
{
private:
	std::map<const int, Client *> _client_map;
	std::string _name;
	std::string _topic;
	std::string _mode;
	std::string _channel_password;
	std::string _operator_password;
	std::vector<int> _operators;
public:
	Channel(std::string name);
	~Channel();

	std::string &get_name();
	std::string &get_topic();
	std::string &get_channel_password();
	std::string get_symbol();
	std::vector<int> &get_operators();
	std::map<const int, Client *> &get_client_map();
	Client *get_client_by_name(std::string client_name);
	Client *get_client_by_fd(const int client_fd);
	void add_client(Client *client);
	void add_operator(Client *client);
	int remove_client(Client *client);
	void remove_operator(Client *client);
	bool is_full();
	bool is_nick_operator(std::string nick);
};

#endif