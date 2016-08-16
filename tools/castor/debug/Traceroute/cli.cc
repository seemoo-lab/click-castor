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
					"\n\t-e|--extensive <x>:"
					"\t\tConsider all invloved nodes."
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

/*
 * Parses all arguments and prints a help-message if an error occurs.
 */
bool CLI::parse_args(int argc, char** argv) {
	if (argc >= 2) {
		struct sockaddr_in tmp_addr;
		int res = inet_pton(AF_INET, argv[1], &(tmp_addr.sin_addr));

		if (res && parse_options(argc, argv)) {
			strcpy(dst_ip, argv[1]);
			return true;
		}
	} 

	print_help();
	return false;
}

/*
 * Parses all options and sets the corresponding attributes in the program.
 * Returns false if a wrong argument was given.
 */ 
bool CLI::parse_options(int argc, char** argv) {
	std::string route_type_str("");
	std::string address_type_str("");
	std::string sort_type_str("");
	
	for (int i=2; i < argc; i++) {
		std::string opt(argv[i]);

		if (opt == "-e" || opt == "--extensive") {
			ext = true;
		} else if (opt == "-d" || opt == "--deadline") {
			deadline = atoi(argv[++i]);
			deadline_flag = true;
		} else if (opt == "-t" || opt == "--timeout") {
			timeout = atoi(argv[++i]);
		} else if (opt == "-I" || opt == "--interface") {
			ifa_name = std::string(argv[++i]);
		} else if (opt == "--ttl") {
			ttl = atoi(argv[++i]);

			if(ttl > MAX_TTL) 
				return false;
		} else if (opt == "--address_type") {
			address_type_str = std::string(argv[++i]);

			if(address_type_str == "mac")	
				address_type = AT_MAC;
			else if(address_type_str == "ip_mac")
				address_type = AT_MAC_IP;
			else 
				address_type = AT_IP;
		} else if (opt == "--route") {
			route_type_str = std::string(argv[++i]);
			
			if(route_type_str == "all") 
				route_type = RT_ALL;
			else if(route_type_str == "nodst")
				route_type = RT_NODST;
			else	
				route_type = RT_DST;
		} else if (opt == "--sort") {
			sort_type_str = std::string(argv[++i]);

			if(sort_type_str == "normal")
				sort_type = ST_NORMAL;
			else if(sort_type_str == "up")
				sort_type = ST_UP;
			else
				sort_type = ST_DOWN;
		} else {
			return false;
		}
	}
	return true;
}

/*
 * Finds the own ip address to a given interface name.
 */
bool CLI::set_local_ip() {
	struct ifaddrs *ifAddrStruct = NULL;
	struct ifaddrs *ifa = NULL;
	void *tmpAddrPtr = NULL;
	char addressBuffer[INET_ADDRSTRLEN];

	// Creates a linked list of structures describing the network interfaces 
	// of the local system, and stores the address of the first item of the 
	// list in *ifap. The list consists of ifaddrs structures
	getifaddrs(&ifAddrStruct);
	
	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr)
			continue;

		if (ifa->ifa_addr->sa_family == AF_INET && ifa->ifa_name == ifa_name) {
			// Is a valid IPv4 Address and desired interface
			tmpAddrPtr=&((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;		
			inet_ntop(AF_INET, tmpAddrPtr, src_ip, INET_ADDRSTRLEN);
			// inet_pton(AF_INET, addressBuffer, &(src_ip.sin_addr));
			return true;
		}
	}
	
	return false;
}

bool CLI::get_ext() {
	return ext;
}

int CLI::get_ttl() {
	return ttl;
}

int CLI::get_deadline() {
	return deadline;
}

bool CLI::contains_deadline() {
	return deadline_flag;
}

int CLI::get_timeout() {
	return timeout;
}

AddressType CLI::get_address_type() {
	return address_type;
}

RouteType CLI::get_route_type() {
	return route_type;
}

SortType CLI::get_sort_type() {
	return sort_type;
}

std::string CLI::get_ifa_name() {
	return ifa_name;
}

char* CLI::get_src_ip() {
	return src_ip;
}

char* CLI::get_dst_ip() {
	return dst_ip;
}
