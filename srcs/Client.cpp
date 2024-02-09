#include "Client.hpp"

Client::Client(int client_fd) : _nick(""), _user(""), _client_fd(client_fd), _registered_nick(false),\
_registered_user(false), _registered(false)
{}

Client::~Client()
{}

void Client::set_readbuf(std::string const &buf)
{
	_readbuf += buf;
}

std::string &Client::get_readbuf() { return _readbuf; }
void Client::set_nick(std::string const &buf) {_nick = buf; }
std::string &Client::get_nick() { return _nick; }
void Client::set_user(std::string const &buf) { _user = buf; }
std::string &Client::get_user() { return _user; }

bool Client::is_nick_registered() { return _registered_nick; }
void Client::set_nick_registered(bool val) { _registered_nick = val; }
bool Client::is_user_registered() { return _registered_user; }
void Client::set_user_registered(bool val) { _registered_nick = val; }
bool Client::is_registered() { return _registered; }
void Client::set_registered(bool val) { _registered = val; }
