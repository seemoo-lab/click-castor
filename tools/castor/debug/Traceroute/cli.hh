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

/*
 * Defines which addresses are to be shown.
 */
enum AddressType { AT_IP, AT_MAC, AT_MAC_IP };

/*
 * Defines which routes are to be shown.
 */
enum RouteType { RT_DST, RT_NODST, RT_ALL }; 

/*
 * Defines how the routes are to be sorted.
 */
enum SortType { ST_NORMAL, ST_UP, ST_DOWN };

class CLI {
public:
	CLI() { };
	~CLI() { };

	// Parses all arguments and prints a help-message if an error occurs.
	bool parse_args(int argc, char** argv);

 	// Finds the own ip address to a given interface name.
	bool set_local_ip();
	
	bool get_ext();
	int get_ttl();
	int get_deadline();
	AddressType get_address_type();
	RouteType get_route_type();
	SortType get_sort_type();
	std::string get_ifa_name();
	char* get_src_ip();
	char* get_dst_ip();
private:

	// Parses all options and sets the corresponding attributes in the program.
	// Returns false if a wrong argument was given.
	bool parse_options(int argc, char** argv);

	// If this attribute is set, all invoveld nodes are considered. 
	bool ext = false;

	// Time to live
	int ttl = MAX_TTL;

	// After x sec the program stops or if -x this attribute is ignored.
	int deadline = -1;

	// Defines which addresses are to be shown.
	AddressType address_type = AT_IP;

	// Defines which routes are to be shown.
	RouteType route_type = RT_DST;

	// Defines how the routes are to be sorted.
	SortType sort_type = ST_NORMAL;

	// The used network interface
	std::string ifa_name = DEFAULT_IFA_NAME;

	// The ip address of the machine that exec. traceroute.
	char src_ip[INET_ADDRSTRLEN];

	// The ip address of the destination.
	char dst_ip[INET_ADDRSTRLEN];
};

#endif
