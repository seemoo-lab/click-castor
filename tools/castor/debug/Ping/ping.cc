#include "ping.hh"
#include <iostream>
#include <iomanip>
#include <string>
#include <sys/socket.h>
#include <numeric>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <signal.h>
#include <iomanip>
#include <sstream>

#define DEBUG_HANDLER_SOCK "/tmp/castor_debug_socket"

bool interrupted = false;

/*
 * ----
 * Ping
 * ----
 * Usage: ping <ip address> [options];
	  Options: 
	  -c|--count <x>:    Sends x packets.
	  -i|--interval <x>: Waits x seconds between sending each packet.
	  -I|--interface <x>:Interface name x.
	  -p|--preloaded <x>:Sends x packets before trying to receive one.
	  -n|--numerical:    Numeric ouput only.
	  -q|--quiet:        Nothing is displayed except the summary lines.
	  -s|--size <x>:     Size of the transported data.
	  -d|--deadline <x>: Timeout.
	  --ttl <x>:         Amount of nodes that are allowed to forwared the packet.
 */
Ping::Ping(int argc, char** argv) {
	// Checks the input
	if (!cli.parse_args(argc, argv))
		return;
	// Determines the own ip address
	if (!cli.set_local_ip()) {
		std::cout << "Error: set_local_ip" << std::endl;
		return;
	}

	std::cout << "PING " << cli.dst_ip << std::endl;

	// Initialize the socket that is used to communicate with castor
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, DEBUG_HANDLER_SOCK);
	len = sizeof(address);

	if(connect(sockfd, (sockaddr*)&address, len) == -1)
		return;

	this->clean_socket();

	if(cli.preloaded)
		this->preloaded();
	// Ping ...
	while(cli.count != transmitted && !interrupted) {
		if(!this->send()) {
			std::cout << "Send failed" << std::endl;
			break;
		} else {
			transmitted++;
			if(!this->receive()) {
				break;
			}
		}	
		sleep(cli.interval);
	}	
	this->analyze();
	close(sockfd);
}

/*
 * It is possible that there is data left on the castor-debug-handler from a previous ping.
 */
void Ping::clean_socket() {
	int n = 0;
	char* buffer;
	int count = 0;
	do {
		ioctl(sockfd, FIONREAD, &count);
		if(!count)
			return;
		std::cout << "clean" << std::endl;
		buffer = (char*)calloc(count, sizeof(char*));

		n = read(sockfd, buffer, count);
		if (n < 0)
			return;
		free(buffer);
	} while(true);
}

/*
 * Sends a command to the connected socket an receives the response.
 */
bool Ping::send_socket_cmd(std::string cmd, std::string& ret) {
	int n = 0;
	char* buffer;
	int count = 0;

	cmd.append("\n");

	if (write(sockfd, cmd.c_str(), cmd.size()) == -1) {
		std::cout << "Error: send_socket_cmd: write" << std::endl;
		return false;
	}

	do {
		ioctl(sockfd, FIONREAD, &count);
		if(!count && n)
			return true;

		buffer = (char*)calloc(count, sizeof(char*));

		n = read(sockfd, buffer, count);
		if (n < 0)
			return false;
		ret += std::string(buffer);
		free(buffer);
	} while(true);
}

/*
 * Sends the debug parameters to the castor-debug-handler via the socket
 */
bool Ping::send() {
	std::string ret("");
	std::string args = "WRITE debug_handler.debug " + std::string(cli.src_ip) + 
			"|" + std::string(cli.dst_ip) + "|" + std::to_string(DBG) + 
			"|" + std::to_string(ARET) + "|" + std::to_string(INSP) + 
			"|" + std::to_string(cli.size) + "|";

	return this->send_socket_cmd(args, ret); 	
}

/*
 * Reiceives the debug informations from the castor-debug-handler via the socket
 */
bool Ping::receive() {
	std::string ret("");
	std::string args = "READ debug_handler.debug";

	int t=cli.deadline;
	bool retval = false;

	do {
		retval = this->send_socket_cmd(args, ret);
		if(!retval) {
			std::cout << "Receive failed" << std::endl;
			return false;
		}
		if(ret.size() <= 55) {
			sleep(1);
			if((--t) == 0) {
				std::cout << "Timeout" << std::endl;
				return false;
			}
			ret = "";
		} else {
			break;
		}
		
	} while(!interrupted); 
	
	print_single_ping_info(ret);

	return true;	
}

/*
 * Sends the desired amount of data before the actual ping
 */
void Ping::preloaded() {
	while(cli.preloaded >= 0 && !interrupted) {
		this->send();
		sleep(cli.interval);
		cli.preloaded--;
	}
}

/*
 * Analyzes the collected data and printd it on the screen
 */
void Ping::analyze() {
	std::cout << std::endl << "--- " << cli.dst_ip << " ping statistics ---" << std::endl;
	int lost_packets = transmitted - times.size();
	int loss_percentage = lost_packets ? (int)(((float)lost_packets/transmitted)*100) : 0;
	auto accumulate_time = times.size() ? accumulate(times.begin(), times.end(), 0.0) : 0;
	auto min_time = times.size() ? *std::min_element(times.begin(), times.end()) : 0; 
	auto max_time = times.size() ? *std::max_element(times.begin(), times.end()) : 0; 
	auto average_time = times.size() ? (float)accumulate_time/(float)times.size() : 0; 	

	std::cout << std::fixed << std::setprecision(2) << transmitted << " packets transmitted, " 
		  << std::to_string(times.size()) << " received, " << std::to_string(loss_percentage)
		  << "\% packet loss, time " << accumulate_time << std::endl << "rtt min/avg/max = "
		  << min_time << "/" << average_time << "/" << max_time << std::endl;
}

/*
 * Parse and print the data that was received during a single ping
 */
void Ping::print_single_ping_info(std::string ret) {
	char* dump = strdup(reinterpret_cast<const char*>(ret.c_str()));
	strtok(dump, "|");
	char* timestamp_str = strtok(NULL, "|");
	char* packet_size_str = strtok(NULL, "|");
	if(timestamp_str && packet_size_str){
		if(!cli.quiet)
			std::cout << std::string(packet_size_str) << " bytes from " 
			<< cli.dst_ip << ": time=" << std::string(timestamp_str) << " ms" << std::endl;
		double timestamp = atof(timestamp_str);
		times.push_back(timestamp);
	} else
		std::cout << " Data was corrupted" << std::endl;
}

void exit_handler(int s) {
	interrupted = true;
}

Ping* ping;

int main(int argc, char** argv) {
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = exit_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	ping = new Ping(argc, argv);
	delete ping;
}

