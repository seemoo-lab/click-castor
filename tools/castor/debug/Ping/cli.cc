#include "cli.hh"
#include <string.h>
#include <string>
#include <iostream>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void print_help() {
	std::string usage 	= 	"Usage: ping <ip address> [options]\n";
	std::string options 	= 	"Options:" 
					"\n\t-c|--count <x>:    \tSends x packets."
					"\n\t-i|--interval <x>: \tWaits x seconds between sending each packet."
					"\n\t-I|--interface <x>:\tInterface name x."
					"\n\t-p|--preloaded <x>:\tSends x packets before trying to receive one."
					"\n\t-q|--quiet:        \tNothing is displayed except the summary lines."
					"\n\t-s|--size <x>:     \tSize of the transported data."
					"\n\t-d|--deadline <x>: \tTimeout."
					"\n\t--ttl <x>:         \tAmount of nodes that are allowed to" 
								"forwared the packet.";
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
 * Parses all arguments and sets the corresponding attributes in the program.
 * Returns false if a wrong argument was given.
 */ 
bool CLI::parse_options(int argc, char** argv) {
	for (int i=2; i < argc; i++) {
		std::string opt(argv[i]);

		if (opt == "-c" || opt == "--count") {
			count = atoi(argv[++i]);
		} else if (opt == "-i" || opt == "--interval") {
			interval = atoi(argv[++i]);
		} else if (opt == "-I" || opt == "--interface") {
			ifa_name = std::string(argv[++i]);
		} else if (opt == "-p" || opt == "--preloaded") {
			num_preloaded = atoi(argv[++i]);
		} else if (opt == "-q" || opt == "--quiet") {
			quiet = true; 
		} else if (opt == "-s" || opt == "--size") {
			size = atoi(argv[++i]);
		} else if (opt == "-d" || opt == "--deadline") {
			deadline = atoi(argv[++i]);
		} else if (opt == "--ttl") {
			ttl = atoi(argv[++i]);

			if(ttl > MAX_TTL) 
				return false;
		}
		else {
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
	// list in *ifap. The list consists of ifaddrs structures.
	getifaddrs(&ifAddrStruct);
	
	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr)
			continue;

		if (ifa->ifa_addr->sa_family == AF_INET && ifa->ifa_name == ifa_name) {
			// is a valid IPv4 Address and desired interface
			tmpAddrPtr=&((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;		
			inet_ntop(AF_INET, tmpAddrPtr, src_ip, INET_ADDRSTRLEN);
			//inet_pton(AF_INET, addressBuffer, &(src_ip.sin_addr));
			return true;
		}
	}

	return false;
}

int CLI::get_count() {
	return count;
}

int CLI::get_interval() {
	return interval;
}

int CLI::get_num_preloaded() {
	return num_preloaded;
}

int CLI::is_quiet() {
	return quiet;
}

int CLI::get_size() {
	return size;
}

int CLI::get_ttl() {
	return ttl;
}

int CLI::get_deadline() {
	return deadline;
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
