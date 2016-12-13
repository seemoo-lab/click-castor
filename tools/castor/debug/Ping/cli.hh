#ifndef CLI_HH
#define CLI_HH
#include <string>
#include <arpa/inet.h>

/*
 * The ttl is limited to 15, because the packet 
 * holds only 4 bits for the ttl.
 */
#define MAX_TTL 0xf

#define DEFAULT_IFA_NAME "wlan0"

class CLI {
public:
	CLI() { };
	~CLI() { };

	// Parses all arguments and prints a help-message if an error occurs
	bool parse_args(int argc, char** argv);

 	// Finds the own ip address to a given interface name
	bool set_local_ip();

	int get_count();
	float get_interval();
	int get_num_preloaded();
	int is_quiet();
	int get_size();
	int get_ttl();
	float get_timeout();
	float get_deadline();
	bool contains_deadline();
	std::string get_ifa_name();
	char* get_src_ip();
	char* get_dst_ip();
private:
	// Parses all options and sets the corresponding attributes in the program.
	// Returns false if a wrong argument was given
	bool parse_options(int argc, char** argv);

	// Number of packets to be send or infinity if -1 
	int count = -1;

	// Number of sec between each packets
	float interval = 1;

	// Number of packets to be send without waiting to replay
	int num_preloaded = 0;

	// Quiet output
	bool quiet = false;

	// Size of the PKT
	int size = 0;

	// Time to live
	int ttl = MAX_TTL;

	// Time to wait for a response, in sec
	float timeout = 1;

	// After x sec the program stops or if -x this attribute is ignored
	float deadline = -1;

	// Is the deadline set to a value other than -1
	bool deadline_flag = false;

	// The used network interface
	std::string ifa_name = DEFAULT_IFA_NAME;

	// The ip address of the machine that exec. traceroute
	char src_ip[INET_ADDRSTRLEN];

	// The ip address of the destination
	char dst_ip[INET_ADDRSTRLEN];
};

#endif
