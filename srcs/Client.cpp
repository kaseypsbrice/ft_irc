#include "Client.hpp"

Client::Client(int client_fd) : _nick(""), _user(""), _client_fd(client_fd), _registered_nick(false),\
_registered_user(false), _registered(false), _password_correct(false), _to_remove(false), _operator(false)
{
	_readbuf.clear();
	_writebuf.clear();
	_old_nick.clear();
	_real.clear();
}

Client::~Client()
{}

void Client::set_readbuf(std::string const &buf) { _readbuf += buf; }
void Client::set_writebuf(std::string const &buf) { _writebuf += buf; }
std::string &Client::get_readbuf() { return _readbuf; }
std::string &Client::get_writebuf() { return _writebuf; }
void Client::set_nick(std::string const &buf) {_nick = buf; }
std::string &Client::get_nick() { return _nick; }
void Client::set_old_nick(std::string const &buf) {_old_nick = buf; }
std::string &Client::get_old_nick() { return _old_nick; }
void Client::set_user(std::string const &buf) { _user = buf; }
std::string &Client::get_user() { return _user; }
void Client::set_real(std::string const &buf) { _real = buf; }
std::string &Client::get_real() { return _real; }
int Client::get_client_fd() { return _client_fd; }

bool Client::is_nick_registered() { return _registered_nick; }
void Client::set_nick_registered(bool val) { _registered_nick = val; }
bool Client::is_user_registered() { return _registered_user; }
void Client::set_user_registered(bool val) { _registered_nick = val; }
bool Client::is_registered() { return _registered; }
void Client::set_registered(bool val) { _registered = val; }
bool Client::is_password_correct() { return _password_correct; }
void Client::set_password_correct(bool val) { _password_correct = val; }
bool Client::is_to_remove() { return _to_remove; }
void Client::set_to_remove(bool val) { _to_remove = val; }
bool Client::is_operator() { return _operator; }
void Client::set_operator(bool val) { _operator = val; }
