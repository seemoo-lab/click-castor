#ifndef PING_HH
#define PING_HH
#include <string>
#include <sys/types.h>
#include <sys/un.h>
#include <vector>
#include "cli.hh"

#define DBG 1
#define ARET 0
#define INSP 0

class Ping {
public:
	Ping(int argc, char** argv);
	~Ping() { };
	CLI cli;	
private:
	// Initialize the socket that is used to communicate with castor.
	bool connect_to_socket();

 	// It is possible that there is data left on the castor-debug-handler from a previous ping.
	void clear_socket();

	// Sends a command to the connected socket an receives the response.
	bool send_socket_cmd(std::string cmd, std::string& ret);

   	// Sends the debug parameters to the castor-debug-handler via the socket
	bool send();

 	// Reiceives the debug informations from the castor-debug-handler via the socket
	bool receive();

	// Sends the desired amount of data before the actual ping
	void preloaded();
	
	// Analyzes the collected data and printd it on the screen
	void analyze();

 	// Parse and print the data that was received during a single ping
	void print_single_ping_info(std::string ret);

	void print_title();

	// Initialize the socket that is used to communicate with castor
	int sockfd;
	int len;
	sockaddr_un address;

	// Stores the rtt of each packet
	std::vector<float> times;

	// Amount of packets that were sent
	int transmitted = 0;
};

#endif
