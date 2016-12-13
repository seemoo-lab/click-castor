#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "../castor_socket.hh"

#define BUF 1024
#define TMP_RESP_MSG "200 Read handler 'debug_handler.ping' OK\nDATA 78\n"



int init_unix_server_socket(const char* dbg_ack_str) {
	int create_socket, new_socket;
	socklen_t addrlen;
	char *buffer = (char*)malloc(BUF);
	ssize_t size;
	struct sockaddr_un address;
	const int y = 1;
	if((create_socket=socket (AF_LOCAL, SOCK_STREAM, 0)) > 0)
		std::cout << "Socket created" << std::endl;
	unlink(CASTOR_SOCKET);
	address.sun_family = AF_LOCAL;
	strcpy(address.sun_path, CASTOR_SOCKET);
	if (bind ( create_socket,
				(struct sockaddr *) &address,
				sizeof (address)) != 0) {
		std::cout << "The Port is in use!" << std::endl;
	}
	listen (create_socket, 5);
	addrlen = sizeof (struct sockaddr_in);
	new_socket = accept ( create_socket,
			(struct sockaddr *) &address,
			&addrlen );
	if (new_socket > 0)
		std::cout << "A Client is connected ..." << std::endl;
	// Ping sends some dbg_pkt_str 
	size = recv(new_socket, buffer, BUF-1, 0);
	if(size > 0)
		buffer[size] = '\0';
	std::cout << "Message received: " << buffer << std::endl;
	std::cout << "Send message: " << "ok" << std::endl;
	send (new_socket, "ok", strlen("ok"), 0);
	
	// Ping wants the dbg_ack_str

	// terminate Traceroute 
	while(1) {
		size = recv(new_socket, buffer, BUF-1, 0);
		if(size > 0)
			buffer[size] = '\0';
		std::cout << "Message received: " << buffer << std::endl;
		std::cout << "Send message: " << dbg_ack_str << std::endl;
		send (new_socket, dbg_ack_str, strlen(dbg_ack_str), 0);
	}

	sleep(2);
	close(new_socket);
	close(create_socket);
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	std::cout << "Ping Test\n===============" << std::endl;
	std::string dbg_ack_str("");
	dbg_ack_str = dbg_ack_str + TMP_RESP_MSG + " |1.9|102|<";
	init_unix_server_socket(dbg_ack_str.c_str());

	return 0;
}
