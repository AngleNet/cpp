#include "Server.h"

#include <iostream>
#include <functional>

int main(void){
	Server server(8000);
	server.start([] (char *buf, int len){
			buf[len-1] = '\0';
			std::cout<<buf<<std::endl;
		});
	return 0;
}
