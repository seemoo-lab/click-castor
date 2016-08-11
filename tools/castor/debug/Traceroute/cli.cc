#include "cli.hh"
#include <string.h>
#include <string>
#include <iostream>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void print_help() {
	std::string usage 	= 	"Usage: traceroute <ip address> [options]\n";
	std::string options 	= 	"Options:" 
					"\n\t-a|--all <x>:"
					"\t\t\tConsider all invloved nodes."
					"\n\t-I|--interface <x>:"
					"\t\tInterface name x."
					"\n\t-d|--deadline<x>:"
					"\t\tMaximal time to wait for a reponse."
					"\n\t--ttl <x>:"
					"\t\t\tAmount of nodes that are allowed to forwared the packet."
					"\n\t--route [all|dst|nodst]:"
					"\tShow all nodes or with(out) destination."
					"\n\t--address_type [ip|mac|ip_mac]:"
					"\tShow node entry as IP, MAC or both."
					"\n\t--sort [normal|up|down]:"
					"\tShows the routes in the desired order.";
	std::cout << usage + options << std::endl;
}

bool CLI::parse_args(int argc, char** argv) {
	if (argc >= 2) {
		struct sockaddr_in tmp_addr;
		int res = inet_pton(AF_INET, argv[1], &(tmp_addr.sin_addr));
		if (res && this->options(argc, argv)) {
			//dst_ip = argv[1];
			strcpy(dst_ip, argv[1]);
			return true;
		}
	} 
	print_help();
	return false;
}

bool CLI::options(int argc, char** argv) {
	std::string route_type_str("");
	std::string address_type_str("");
	std::string sort_type_str("");
	for (int i=2; i < argc; i++) {
		std::string opt(argv[i]);
		if (opt == "-a" || opt == "--all") {
			all = true;
		} else if (opt == "-d" || opt == "--deadline") {
			deadline = atoi(argv[++i]);
		} else if (opt == "-I" || opt == "--interface") {
			ifa_name = std::string(argv[++i]);
		} else if (opt == "--ttl") {
			ttl = atoi(argv[++i]);
		} else if (opt == "--address_type") {
			address_type_str = std::string(argv[++i]);
			if(address_type_str == "mac")	
				at = AT_MAC;
			else if(address_type_str == "ip_mac")
				at = AT_MAC_IP;
			else 
				at = AT_IP;
		} else if (opt == "--route") {
			route_type_str = std::string(argv[++i]);
			if(route_type_str == "all") 
				rt = RT_ALL;
			else if(route_type_str == "nodst")
				rt = RT_NODST;
			else	
				rt = RT_DST;
		} else if (opt == "--sort") {
			sort_type_str = std::string(argv[++i]);
			if(sort_type_str == "normal")
				st = ST_NORMAL;
			else if(sort_type_str == "up")
				st = ST_UP;
			else
				st = ST_DOWN;
		} else {
			return false;
		}
	}
	return true;
}

bool CLI::set_local_ip() {
	struct ifaddrs *ifAddrStruct = NULL;
	struct ifaddrs *ifa = NULL;

	getifaddrs(&ifAddrStruct);
	
	void *tmpAddrPtr = NULL;
	char addressBuffer[INET_ADDRSTRLEN];
	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr)
			continue;
		if (ifa->ifa_addr->sa_family == AF_INET && ifa->ifa_name == ifa_name) {
			// is a valid IPv4 Address and desired interface
			tmpAddrPtr=&((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;		
			inet_ntop(AF_INET, tmpAddrPtr, src_ip, INET_ADDRSTRLEN);
			// inet_pton(AF_INET, addressBuffer, &(src_ip.sin_addr));
			return true;
		}
	}
	return false;
}
