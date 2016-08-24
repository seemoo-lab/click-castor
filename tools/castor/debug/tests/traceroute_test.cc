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

#define BUF 1024
#define UDS_FILE "/tmp/castor_debug_socket"
#define TMP_RESP_MSG "200 Read handler 'debug_handler.ping' OK\nDATA 78\n" 

std::string IPs[] = {"192.168.56.100", "192.168.56.101", "192.168.56.102", "192.168.56.103", "192.168.56.104",
			"192.168.56.105","192.168.56.106","192.168.56.107","192.168.56.108","192.168.56.109"};
std::string MACs[] = {"08-00-27-CC-77-50", "08-00-27-CC-77-51", "08-00-27-CC-77-52", "08-00-27-CC-77-53", 
			"08-00-27-CC-77-54", "08-00-27-CC-77-55", "08-00-27-CC-77-56", "08-00-27-CC-77-57", 
				"08-00-27-CC-77-58", "08-00-27-CC-77-59"}; 

int init_unix_server_socket(const char* dbg_ack_str) {
	int create_socket, new_socket;
	socklen_t addrlen;
	char *buffer = (char*)malloc(BUF);
	ssize_t size;
	struct sockaddr_un address;
	const int y = 1;

	if((create_socket=socket (AF_LOCAL, SOCK_STREAM, 0)) > 0)
		std::cout << "Socket created" << std::endl;
	unlink(UDS_FILE);
	address.sun_family = AF_LOCAL;
	strcpy(address.sun_path, UDS_FILE);
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

	// Traceroute sends some clear message
	size = recv(new_socket, buffer, BUF-1, 0);
	if(size > 0)
		buffer[size] = '\0';
	std::cout << "Message received: " << buffer << std::endl;
	std::cout << "Send message: " << "ok" << std::endl;
	send (new_socket, "ok", strlen("ok"), 0);

	// Traceroute sends some dbg_pkt_str 
	size = recv(new_socket, buffer, BUF-1, 0);
	if(size > 0)
		buffer[size] = '\0';
	std::cout << "Message received: " << buffer << std::endl;
	std::cout << "Send message: " << "ok" << std::endl;
	send (new_socket, "ok", strlen("ok"), 0);
	
	// Traceroute wants the dbg_ack_str
	size = recv(new_socket, buffer, BUF-1, 0);
	if(size > 0)
		buffer[size] = '\0';
	std::cout << "Message received: " << buffer << std::endl;
	sleep(1);
	std::cout << "Send message: " << dbg_ack_str << std::endl;
	send (new_socket, dbg_ack_str, strlen(dbg_ack_str), 0);

	// terminate Traceroute 
	while(1) {
		size = recv(new_socket, buffer, BUF-1, 0);
		if(size > 0)
			buffer[size] = '\0';
		std::cout << "Message received: " << buffer << std::endl;
		sleep(1);
		std::cout << "Send message: " << std::endl;
		send (new_socket, " ", strlen(" "), 0);
		sleep(2);
	}

	sleep(2);
	close(new_socket);
	close(create_socket);
	return EXIT_SUCCESS;
}

std::string create_graph1() {
	std::stringstream ss;
	ss << TMP_RESP_MSG << " |1.632|102|" << MACs[2] << ":" << IPs[2] << "," 
					    << MACs[1] << ":" << IPs[1] << "," 
			  		    << MACs[0] << ":" << IPs[0] << "|<"
	   << TMP_RESP_MSG << " |0.532|102|" << MACs[4] << ":" << IPs[4] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<"
    	   << TMP_RESP_MSG << " |0.932|102|" << MACs[1] << ":" << IPs[1] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<";
	return ss.str();
}

std::string create_graph2() {
	std::stringstream ss;
	ss << TMP_RESP_MSG << " |1.632|102|" << MACs[2] << ":" << IPs[2] << "," 
					    << MACs[1] << ":" << IPs[1] << "," 
			  		    << MACs[0] << ":" << IPs[0] << "|<"
			   << " |0.532|102|" << MACs[4] << ":" << IPs[4] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<"
			   << " |0.932|102|" << MACs[1] << ":" << IPs[1] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<"
			   << " |0.2|102|"   << MACs[3] << ":" << IPs[3] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<"
			   << " |1.0|102|"   << MACs[1] << ":" << IPs[1] << ","
			  		    << MACs[3] << ":" << IPs[3] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<"
			   << " |2.2|102|"   << MACs[2] << ":" << IPs[2] << ","
			  		    << MACs[1] << ":" << IPs[1] << ","
			  		    << MACs[3] << ":" << IPs[3] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<";
	return ss.str();
}

std::string create_graph3() {
	std::stringstream ss;
	ss << TMP_RESP_MSG << " |1.632|102|" << MACs[2] << ":" << IPs[2] << "," 
			  		    << MACs[0] << ":" << IPs[0] << "|<"
			   << " |0.532|102|" << MACs[4] << ":" << IPs[4] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<"
			   << " |0.932|102|" << MACs[1] << ":" << IPs[1] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<";
	return ss.str();
}

std::string create_graph4() {
	std::stringstream ss;
	ss << TMP_RESP_MSG << " |1.632|102|" << MACs[2] << ":" << IPs[2] << "," 
					    << MACs[1] << ":" << IPs[1] << "," 
			  		    << MACs[0] << ":" << IPs[0] << "|<"
			   << " |2.63|102|" << MACs[3] << ":" << IPs[3] << "," 
					    << MACs[1] << ":" << IPs[1] << "," 
			  		    << MACs[0] << ":" << IPs[0] << "|<"
			   << " |0.932|102|"<< MACs[1] << ":" << IPs[1] << ","
			  		    << MACs[0] << ":" << IPs[0] << "|<";
	return ss.str();
}

int main(int argc, char** argv) {
	std::cout << "Traceroute Test\n===============" << std::endl;
	std::string dbg_ack_str("");
	int num_graphs = 4;
	int selected_graph = 1;
	
	if(argc == 2) {
		selected_graph = atoi(argv[1]);
		selected_graph = selected_graph <= num_graphs ? selected_graph : 1;
	}

	switch(selected_graph) {
	case 1:
		dbg_ack_str = create_graph1();
		break;
	case 2:
		dbg_ack_str = create_graph2();
		break;
	case 3:
		dbg_ack_str = create_graph3();
		break;
	case 4:
		dbg_ack_str = create_graph4();
		break;
	default:
		dbg_ack_str = create_graph1();
	}
	init_unix_server_socket(dbg_ack_str.c_str());

	return 0;
}
