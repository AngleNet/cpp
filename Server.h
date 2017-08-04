#include <sys/epoll.h>
#include <functional>

#define EPOLL_MAX_EVENTS 100000
#define SOCK_MAX_CONN 100000
#define NET_MAX_HOST 255
#define NET_MAX_SERVER 255

class Server{
public:
	Server(unsigned int port);
	~Server();
	void start();
private:
	Server(const Server&);
	Server& operator=(const Server& o);

	int makeNonBlocking(int fd);
	int event_fd;
	int sock_fd;
	struct epoll_event *events;
	struct epoll_event event;
};
