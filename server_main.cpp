#include "Server.h"

#include <iostream>

int main(void){
	Server server(8000);
	server.start();
	return 0;
}
