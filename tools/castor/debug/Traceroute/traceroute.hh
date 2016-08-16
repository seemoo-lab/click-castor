#ifndef TRACEROUTE_HH 
#define TRACEROUTE_HH #include <string> #include <sys/types.h>
#include <sys/un.h>
#include <vector>
#include "cli.hh"
#include "route.hh"

#define DBG 1
#define ARET 0
#define INSP 1

class Traceroute {
public:
	Traceroute(int argc, char** argv);
	~Traceroute() { };

	// Parse and stores all user inputs
	CLI cli;	
private:
	// Initialize the socket that is used to communicate with castor
	bool connect_to_socket();	

 	// It is possible that there is data left on 
	// the castor-debug-handler from a previous ping
	void clear_socket();

	// Sends a command to the connected socket an receives the response
	bool send_socket_cmd(std::string cmd, std::string& ret);

	// Sends the debug parameters to the castor-debug-handler via the socket
	bool send();

	// Reiceives the debug informations from the castor-debug-handler via the socket
	bool receive();

	// Parse the data that was received
	void parse_single_traceroute_info(std::string ret); 

	// If all castor nodes return their paths there will be some duplicates that must be deleted 
	void merge_routes();

	// Sorts the routes in order of the rtt
	void sort_routes();

	// Analyzes the collected data and printd it on the screen
	void analyze_routes();
	
	// Prints one single Route with time, size and num_nodes
	void setup_print_route(std::string& routes_str, Route* route);

	void print_title();

	// Socket that is used to communicate with castor
	int sockfd;
	int len;
	sockaddr_un address;

	// Is true if one route contains the destination node
	bool dst_found = false;

	// Stores all received routes 
	std::vector<Route> routes;	
};

#endif
