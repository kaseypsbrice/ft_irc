#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name), _capacity(-1)
{
	_channel_password.clear();
	_operator_password.clear();
	_mode.clear();
	_topic.clear();
	_operators.clear();
	_client_map.clear();
	_whitelist.clear();
}

Channel::~Channel()
{}

std::string &Channel::get_name() { return _name; }
std::string &Channel::get_topic() { return _topic; }
std::string &Channel::get_channel_password() { return _channel_password; }
std::string &Channel::get_mode() { return _mode; }
void Channel::set_topic(std::string topic) { _topic = topic; }
std::vector<int> &Channel::get_operators() { return _operators; }
std::map<const int, Client *> &Channel::get_client_map() { return _client_map; }
void Channel::set_channel_password(std::string pass) { _channel_password = pass; }
void Channel::remove_channel_password() {_channel_password.clear(); }
int	&Channel::get_capacity() { return _capacity; }
void Channel::set_capacity(int val) { _capacity = val; }

Client *Channel::get_client_by_name(std::string client_name)
{
	std::map<const int, Client *>::iterator	it;

	for (it = _client_map.begin(); it != _client_map.end(); it++)
	{
		if (it->second->get_nick() == client_name)
			return it->second;
	}
	return NULL;
}

Client *Channel::get_client_by_fd(const int client_fd)
{
	std::map<const int, Client *>::iterator	it;

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
	{
		_operators.erase(it);
	}
}

int Channel::remove_client(Client *client)
{
	if (_client_map.find(client->get_client_fd()) != _client_map.end())
	{
		remove_whitelist(client);
		remove_operator(client);
		_client_map.erase(client->get_client_fd());
		return 1;
	}
	return 0;
}

bool Channel::is_full()
{
	if (_capacity == -1)
		return false;
	if ((int)_client_map.size() >= _capacity)
		return true;
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

bool Channel::is_whitelisted(Client *client)
{
	std::vector<int>::iterator it;
	it = std::find(_whitelist.begin(), _whitelist.end(), client->get_client_fd());
	if (it != _whitelist.end())
		return true;
	return false;
}

void Channel::add_whitelist(Client *client)
{
	if (!is_whitelisted(client))
		_whitelist.push_back(client->get_client_fd());
}

void Channel::remove_whitelist(Client *client)
{
	std::vector<int>::iterator it;
	it = std::find(_whitelist.begin(), _whitelist.end(), client->get_client_fd());
	if (it != _whitelist.end())
		_whitelist.erase(it);
}

// mostly useless as 'p' and 's' modes arent implemented
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

void	Channel::add_mode(std::string const mode)
{
	if (_mode.empty() == true)
		_mode = "+" + mode;
	else 
		_mode += mode;
}

void	Channel::remove_mode(std::string const mode)
{
	size_t pos = _mode.find(mode);
	_mode.erase(pos, 1);
}

void	Channel::broadcast_string(std::string str)
{
	std::map<const int, Client *>::iterator it;

	for (it = _client_map.begin(); it != _client_map.end(); it++)
	{
		it->second->set_writebuf(str);
	}
}
