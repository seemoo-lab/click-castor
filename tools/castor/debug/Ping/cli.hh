#ifndef CLI_HH
#define CLI_HH
#include <string>
#include <arpa/inet.h>

class CLI {
public:
	CLI() { };
	~CLI() { };

	bool parse_args(int argc, char** argv);
	bool options(int argc, char** argv);
	bool set_local_ip();

	int count = -1;
	int interval = 2;
	int preloaded = 0;
	bool numerical = false;
	bool quiet = false;
	int size = 0;
	int ttl = -1;
	int deadline = -1;
	std::string ifa_name = "eth0";
	char src_ip[INET_ADDRSTRLEN];
	char dst_ip[INET_ADDRSTRLEN];
};

#endif
