#include "Server.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <cstring>
#include <functional>
#include <iostream>

Server::Server(unsigned int port){
	struct addrinfo hints;
	struct addrinfo* res, *rp;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int rc = getaddrinfo(NULL, std::to_string((long long)port).c_str(), &hints, &res);
	if (rc < 0){
		std::cout<<"Failed to get address information"<<std::endl;
		return;
	}
	for (rp = res; rp != NULL; rp = rp->ai_next){
		sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sock_fd < 0){
			std::cout<<"Failed to create socket"<<std::endl;
			continue;
		}
		rc = bind(sock_fd, rp->ai_addr, rp->ai_addrlen);
		if (rc == 0){
			break;
		}
		close(sock_fd);
	} 
	freeaddrinfo(res);
	
	//Set non-blocking
	int flags = 0;
	flags = fcntl(sock_fd, F_GETFL, 0);
	if (flags < 0){
		std::cout<<"Failed to get file flags"<<std::endl;
	}
	flags |= O_NONBLOCK;
	rc = fcntl(sock_fd, F_SETFL, flags);
	if (rc < 0){
		std::cout<<"Failed to set NON-BLOCK"<<std::endl;
	}

	//Listen
	rc = listen(sock_fd, SOCK_MAX_CONN);
	if (rc < 0){
		std::cout<<"Failed to listen"<<std::endl;
	}
	
	event_fd = epoll_create1(EPOLL_CLOEXEC);
	if (event_fd < 0){
		std::cout<<"Failed to create epoll instance"<<std::endl;
	}
	
	event.data.fd = sock_fd;
	event.events = EPOLLIN | EPOLLET;
	
	rc = epoll_ctl(event_fd, EPOLL_CTL_ADD, sock_fd, &event);
	if (rc < 0){
		std::cout<<"Failed to add socket to EPOLL"<<std::endl;
	}
	
	events = new struct epoll_event[EPOLL_MAX_EVENTS];
}
Server::~Server(){
	close(sock_fd);
	close(event_fd);
	delete[] events;
}
void Server::start(std::function<void(char *, int)> handler){
	for(;;){
		int num_events = 0, i;
		num_events = epoll_wait(event_fd, events, EPOLL_MAX_EVENTS, -1);
		for(i = 0; i < num_events; ++i){
			if((events[i].events & EPOLLERR) ||
			   (events[i].events & EPOLLHUP)){
				std::cout<<"Epoll error"<<std::endl;
				continue;
			}
			if(events[i].data.fd == sock_fd)  
				for (;;){
					struct sockaddr in_addr;
					socklen_t len = sizeof(struct sockaddr);
					int fd = 0;
					char host_buf[NET_MAX_HOST], server_buf[NET_MAX_SERVER];
					fd = accept(sock_fd, &in_addr, &len);
					if (fd < 0){
						if ((errno == EAGAIN) ||
							(errno == EWOULDBLOCK)){
							break;
						}else{
							std::cout<<"Error while accepting"<<std::endl;
							break;
						}
					}
					int rc = 0;
					rc = getnameinfo(&in_addr, len, host_buf, sizeof(host_buf),
									 server_buf, sizeof(server_buf), 
									 NI_NUMERICHOST | NI_NUMERICSERV);
					if (rc == 0){
						std::cout<<"Accepting connection on file descriptor " <<fd
								 <<", from " << host_buf <<", " << server_buf<<std::endl;
					}
					makeNonBlocking(fd);
					memset(&event, 0, sizeof(struct epoll_event));
					event.data.fd = fd;
					event.events = EPOLLIN | EPOLLET;
					rc = epoll_ctl(event_fd, EPOLL_CTL_ADD, fd, &event);
					if (rc < 0){
						std::cout<<"Failed to bind to EPOLL"<<std::endl;
						close(fd);
					}
					continue;
				}else{
					for(;;){
						ssize_t count;
						char buf[512];
						int done = 0;
						count = read(events[i].data.fd, buf, sizeof(buf));
						if(count == -1){
							if(errno != EAGAIN){
								std::cout<<"Read error"<<std::endl;
								done = 1;
							}
							break;
						}else if(count == 0){
							done = 1;
							break;
						}
						int	rc = 0;
						rc = write(1, buf, count);
						if(done == 1){
							close(events[i].data.fd);
						}
					}
				}
			}
		}
	}
}
Server::Server(const Server& o){}
Server& operator=(const Server& o){}

int Server::makeNonBlocking(int fd){
	int rc = 0, flags = 0;
	flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0){
		std::cout<<"Error to get file flags"<<std::endl;
	}
	flags |= O_NONBLOCK;
	rc = fcntl(fd, F_SETFL, flags);
	if (rc < 0){
		std::cout<<"Failed to set NON-BLOCK"<<std::endl;
	}
	return rc;
}
