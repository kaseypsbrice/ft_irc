#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class Client
{
private:
	std::string _readbuf;
	std::string	_writebuf;
	std::string	_nick;
	std::string	_old_nick;
	std::string	_user;
	int			_client_fd;
	bool		_registered_nick;
	bool		_registered_user;
	bool		_registered;

public:
	Client(int client_fd);
	~Client();

	void set_readbuf(std::string const &buf);
	std::string &get_readbuf();
	void set_writebuf(std::string const &buf);
	std::string &get_writebuf();
	void set_nick(std::string const &buf);
	std::string &get_nick();
	void set_old_nick(std::string const &buf);
	std::string &get_old_nick();
	void set_user(std::string const &buf);
	std::string &get_user();
	bool is_nick_registered();
	void set_nick_registered(bool val);
	bool is_user_registered();
	void set_user_registered(bool val);
	bool is_registered();
	void set_registered(bool val);
};


#endif