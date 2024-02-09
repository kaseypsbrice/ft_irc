# include "irc.hpp"

int	Server::handle_existing_client(std::vector<pollfd>& poll_fds, std::vector<pollfd>::iterator &it)
{
	Client	*client;
	client = get_client(it->fd); // retrieve client from map
	char message[MESSAGE_BUF]; // buffer for recv
	int size; // holds message size

	(void)poll_fds;
	memset(message, 0, sizeof(message));
	size = recv(it->fd, message, MESSAGE_BUF, 0);

	if (size < 0)
	{
		std::cerr << "Recv failed" << std::endl;
		return 1;
	}
	if (size == 0)
	{
		// client disconnected
		return 1;
	}
	std::cout << "Recieved Message:" << std::endl << message;
	// messages are buffered in case of partial data transfer
	client->set_readbuf(message);
	return 0;
}