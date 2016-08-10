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
					"\n\t-n|--numerical:    \tNumeric ouput only."
					"\n\t-q|--quiet:        \tNothing is displayed except the summary lines."
					"\n\t-s|--size <x>:     \tSize of the transported data."
					"\n\t-d|--deadline <x>: \tTimeout."
					"\n\t--ttl <x>:         \tAmount of nodes that are allowed to" 
								"forwared the packet.";
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
	for (int i=2; i < argc; i++) {
		std::string opt(argv[i]);
		if (opt == "-c" || opt == "--count") {
			count = atoi(argv[++i]);
		} else if (opt == "-i" || opt == "--interval") {
			interval = atoi(argv[++i]);
		} else if (opt == "-I" || opt == "--interface") {
			ifa_name = std::string(argv[++i]);
		} else if (opt == "-p" || opt == "--preloaded") {
			preloaded = atoi(argv[++i]);
		} else if (opt == "-n" || opt == "--numerical") {
			numerical = true; 
		} else if (opt == "-q" || opt == "--quiet") {
			quiet = true; 
		} else if (opt == "-s" || opt == "--size") {
			size = atoi(argv[++i]);
		} else if (opt == "-d" || opt == "--deadline") {
			deadline = atoi(argv[++i]);
		} else if (opt == "--ttl") {
			ttl = atoi(argv[++i]);
		}
		else {
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
			//inet_pton(AF_INET, addressBuffer, &(src_ip.sin_addr));
			return true;
		}
	}
	return false;
}
