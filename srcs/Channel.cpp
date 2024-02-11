#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name)
{
	_channel_password.clear();
	_operator_password.clear();
}

Channel::~Channel()
{}

std::string &Channel::get_name() { return _name; }
std::string &Channel::get_topic() { return _topic; }
std::string &Channel::get_channel_password() { return _channel_password; }
std::vector<int> &Channel::get_operators() { return _operators; }
std::map<const int, Client *> &Channel::get_client_map() { return _client_map; }

Client *Channel::get_client_by_name(std::string client_name)
{
	std::map<int, Client *>::iterator	it;

	for (it = _client_map.begin(); it != _client_map.end(); it++)
	{
		if (it->second->get_nick() == client_name)
			return it->second;
	}
	return NULL;
}

Client *Channel::get_client_by_fd(const int client_fd)
{
	std::map<int, Client *>::iterator	it;

	it = _client_map.find(client_fd);
	if (it == _client_map.end())
		return NULL;
	return it->second;
}

void Channel::add_client(Client *client)
{
	if (get_client_by_fd(client->get_client_fd()) != NULL)
		return ;
	_client_map.insert(std::pair<const int, Client *>(client->get_client_fd(), client));
	std::cout << client->get_nick() << " added to " << _name << std::endl;
}

void Channel::add_operator(Client *client)
{
	std::vector<int>::iterator it;

	it = std::find(_operators.begin(), _operators.end(), client->get_client_fd());
	if (it != _operators.end())
		return ;
	_operators.push_back(client->get_client_fd());
	std::cout << client->get_nick() << " added to operator " << _name << std::endl;
}

void Channel::remove_operator(Client *client)
{
	std::vector<int>::iterator it;

	it = std::find(_operators.begin(), _operators.end(), client->get_client_fd());
	if (it != _operators.end())
		_operators.erase(it);
}

void Channel::remove_client(Client *client)
{
	_client_map.erase(client->get_client_fd());
}

bool Channel::is_full()
{
	// todo ~this~
	return false;
}

bool Channel::is_nick_operator(std::string nick)
{
	Client *client = get_client_by_name(nick);

	if (client == NULL)
		return false;
	std::vector<int>::iterator it;
	it = std::find(_operators.begin(), _operators.end(), client->get_client_fd());
	if (it != _operators.end())
		return true;
	return false;
}

std::string Channel::get_symbol()
{
	std::string symbol;

	if (_mode.find('s') != std::string::npos)
		symbol += "@";
	else if (_mode.find('p') != std::string::npos)
		symbol += "*";
	else
		symbol += "=";
	return symbol;
}