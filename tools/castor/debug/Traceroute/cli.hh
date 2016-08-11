#ifndef CLI_HH
#define CLI_HH
#include <string>
#include <arpa/inet.h>
#include "route.hh"

enum SortType { ST_NORMAL, ST_UP, ST_DOWN };

class CLI {
public:
	CLI() { };
	~CLI() { };

	bool parse_args(int argc, char** argv);
	bool options(int argc, char** argv);
	bool set_local_ip();

	bool all = false;
	int ttl = -1;
	int deadline = -1;
	AddressType at = AT_IP;
	RouteType rt = RT_DST;
	SortType st = ST_NORMAL;
	std::string ifa_name = "wlan0";
	char src_ip[INET_ADDRSTRLEN];
	char dst_ip[INET_ADDRSTRLEN];
	char *src_mac = NULL;
};

#endif
