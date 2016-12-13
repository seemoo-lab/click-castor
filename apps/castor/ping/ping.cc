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
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
#include <iomanip>
#include <sstream>
#include "../castor_socket.hh"

// This message is always send "200 Read handler 'debug_handler.ping' OK\nDATA 78\n"
#define DEF_SOCK_MSG_LEN 55

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
	  -q|--quiet:        Nothing is displayed except the summary lines.
	  -s|--size <x>:     Size of the transported data.
	  -t|--timeout<x>:   Time to wait for a response, in sec.
	  -d|--deadline <x>: Specifiy a timeout, in sec, before ping exits.
	  --ttl <x>:         Amount of nodes that are allowed to forwared the packet.
 */
Ping::Ping(int argc, char** argv) {
	struct timeval start_time, curr_time;

	// Checks the input
	if (!cli.parse_args(argc, argv))
		return;
	
	// Determines the own ip address
	if (!cli.set_local_ip()) {
		std::cout << "Error: set_local_ip" << std::endl;
		return;
	}

	print_title();

	if(!connect_to_socket()) {
		std::cout << "Error: Could not connect to the " << 
			CASTOR_SOCKET << " socket." << std::endl;
		return;
	}

	clear_socket();
	gettimeofday(&start_time, NULL);

	if(cli.get_num_preloaded())
		preloaded();

	// Ping ...
	while(cli.get_count() != transmitted && !interrupted) {
		if(!send()) {
			std::cout << "Error: Could not send message." << std::endl;
			break;
		} else {
			transmitted++;

			if(!receive()) {
				std::cout << "Error: Could not receive message." << std::endl;
				break;
			}
		}	

		// Check deadline
		if(cli.contains_deadline()) {
			gettimeofday(&curr_time, NULL);

			if((curr_time.tv_sec-start_time.tv_sec) >= cli.get_deadline())
				break;
		}

		usleep(cli.get_interval()*1000000);
	}	

	analyze();
	close(sockfd);
}

/*
 * Initialize the socket that is used to communicate with castor.
 */
bool Ping::connect_to_socket() {
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, CASTOR_SOCKET);
	len = sizeof(address);

	if(connect(sockfd, (sockaddr*)&address, len) == -1)
		return false;

	return true;
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
		std::cout << "Error: Writing to socket failed." << std::endl;
		return false;
	}

	do {
		ioctl(sockfd, FIONREAD, &count);
		
		if(!count && n)
			return true;

		buffer = (char*)calloc(count, sizeof(char*));
		n = read(sockfd, buffer, count);

		if (n < 0) {
			std::cout << "Error: Reading from socket failed." << std::endl;
			return false;
		}

		ret += std::string(buffer);
		free(buffer);
	} while(true);
}

/*
 * It is possible that there is data left on the castor-debug-handler from a previous ping.
 */
void Ping::clear_socket() {
	std::string ret("");
	std::string args = "READ debug_handler.clear"; 
	send_socket_cmd(args, ret); 	
}
/*
 * Sends the debug parameters to the castor-debug-handler via the socket.
 */
bool Ping::send() {
	std::string ret("");
	std::string args = "WRITE debug_handler.debug " + std::string(cli.get_src_ip()) + 
			"|" + std::string(cli.get_dst_ip()) + "|" + std::to_string(DBG) + 
			"|" + std::to_string(ARET) + "|" + std::to_string(INSP) + 
			"|" + std::to_string(cli.get_ttl()) + "|" + std::to_string(cli.get_size()) + "|";

	return send_socket_cmd(args, ret); 	
}

/*
 * Reiceives the debug informations from the castor-debug-handler via the socket.
 */
bool Ping::receive() {
	std::string ret("");
	std::string args = "READ debug_handler.debug";

	float t=cli.get_timeout();
	bool retval = false;

	do {
		retval = send_socket_cmd(args, ret);

		if(!retval)
			return false;

		if(ret.size() <= DEF_SOCK_MSG_LEN) {
			usleep(1000);

			// Check the timeout
			t = t - 0.001;
			if(t <= 0) {
				if(!cli.is_quiet())
					std::cout << "From " << cli.get_dst_ip()
						  << " Destination Host Unreachable" << std::endl;
				return true;
			}

			ret = "";
		} else {
			// Got a debug_ack_str
			break;
		}
		
	} while(!interrupted); 
	
	//std::cout << ret << std::endl;
	print_single_ping_info(ret);

	return true;	
}

/*
 * Sends the desired amount of data before the actual ping.
 */
void Ping::preloaded() {
	int n = cli.get_num_preloaded();

	while(n >= 0 && !interrupted) {
		send();
		usleep(cli.get_interval()*1000000);
		n--;
	}
}

/*
 * Analyzes the collected data and printd it on the screen.
 */
void Ping::analyze() {
	std::cout << std::endl << "--- " << cli.get_dst_ip() << " ping statistics ---" << std::endl;
	int lost_packets = transmitted - times.size();
	int loss_percentage = lost_packets ? (int)(((float)lost_packets/transmitted)*100) : 0;
	auto accumulate_time = times.size() ? accumulate(times.begin(), times.end(), 0.0) : 0;
	auto min_time = times.size() ? *std::min_element(times.begin(), times.end()) : 0; 
	auto max_time = times.size() ? *std::max_element(times.begin(), times.end()) : 0; 
	auto average_time = times.size() ? (float)accumulate_time/(float)times.size() : 0; 	

	std::cout << std::fixed << std::setprecision(2) << transmitted << " packets transmitted, " 
		  << std::to_string(times.size()) << " received, " << std::to_string(loss_percentage)
		  << "\% packet loss, time " << accumulate_time << "ms" << std::endl << "rtt min/avg/max = "
		  << min_time << "/" << average_time << "/" << max_time << std::endl;
}

/*
 * Parse and print the data that was received during a single ping.
 */
void Ping::print_single_ping_info(std::string ret) {
	char* dump = strdup(reinterpret_cast<const char*>(ret.c_str()));
	strtok(dump, "|");
	char* timestamp_str = strtok(NULL, "|");
	char* packet_size_str = strtok(NULL, "|");
	float timestamp;
	
	if(timestamp_str && packet_size_str){
		timestamp = atof(timestamp_str);
		if(!cli.is_quiet())
			std::cout << std::fixed << std::setprecision(2) 
				  << packet_size_str << " bytes from " 
			 	  << cli.get_dst_ip() << ": time=" << timestamp 
				  << " ms" << std::endl;

		times.push_back(timestamp);
	} else
		std::cout << " Data was corrupted" << std::endl;
}

void Ping::print_title() {
	std::cout << "PING " << cli.get_dst_ip() << std::endl;
}

// Is called by the interrupt handler, if 'Strg + c' was pressed.
void exit_handler(int s) {
	interrupted = true;
}

Ping* ping;

int main(int argc, char** argv) {
	// With the sigIntHandler we can interrupt the program with 'Strg + c'
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = exit_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	ping = new Ping(argc, argv);
	delete ping;
}

