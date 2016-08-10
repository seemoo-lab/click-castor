#include "traceroute.hh"
#include <iostream>
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
 * ----------
 * Traceroute 
 * ----------
 *
 * Usage: traceroute <ip address> [options]
	  Options:
	  -a|--all <x>:      Consider all invloved nodes.
	  -I|--interface <x>:Interface name x.
	  -d|--deadline<x>:  Maximal time to wait for a reponse.
	  --ttl <x>:         Amount of nodes that are allowed to forwared the packet.
 	  --route [all|dst|nodst]:  Show all nodes or with(out) destination.
	  --address_type [ip|mac|ip_mac]: Show node entry as IP, MAC or both.
 */
Traceroute::Traceroute(int argc, char** argv) {
	// Checks the input
	if (!cli.parse_args(argc, argv))
		return;
	
	// Determines the own ip address
	if (!cli.set_local_ip()) {
		std::cout << "Error: set_local_ip" << std::endl;
		return;
	}

	std::cout << "TRACEROUTE\n==============================================\n\n" 
		  << "   -- " << cli.src_ip << " ==> " << cli.dst_ip <<  " --" << std::endl;

	// Initialize the socket that is used to communicate with castor
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, DEBUG_HANDLER_SOCK);
	len = sizeof(address);

	if(connect(sockfd, (sockaddr*)&address, len) == -1)
		return;

	this->clean_socket();

	// Kick off traceroute
	if(!this->send()) {
		std::cout << "Send failed" << std::endl;
		return;
	}
	// Receives all responses
	if(!this->receive()) {
		std::cout << "Receive failed" << std::endl;
		return;
	}

	// Removes the duplicates
	this->merge_routes();

	this->sort_routes();

	this->analyze();
	close(sockfd);
}

/*
 * It is possible that there is data left on the castor-debug-handler from a previous ping.
 */
void Traceroute::clean_socket() {
	int n = 0;
	char* buffer;
	int count = 0;
	do {
		ioctl(sockfd, FIONREAD, &count);
		if(!count)
			return;
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
bool Traceroute::send_socket_cmd(std::string cmd, std::string& ret) {
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
		if(count == 0 && n != 0){
			return true;
		}
		
		buffer = (char*)calloc(count, sizeof(char*));

		n = read(sockfd, buffer, count);
		if (n < 0) {
			std::cout << "read failed" << std::endl;
			free(buffer);
			return false;
		}
		ret += std::string(buffer);
		free(buffer);
	} while(true);
}

/*
 * Sends the debug parameters to the castor-debug-handler via the socket
 */
bool Traceroute::send() {
	std::string ret("");
	std::string args = "WRITE debug_handler.debug " + std::string(cli.src_ip) + 
			"|" + std::string(cli.dst_ip) + "|" + std::to_string(DBG) + 
			"|" + std::to_string(cli.all) + "|" + std::to_string(INSP) +
			"|0|";

	return this->send_socket_cmd(args, ret); 	
}

/*
 * Reiceives the debug informations from the castor-debug-handler via the socket
 */
bool Traceroute::receive() {
	std::string tmp_ret("");
	std::string ret(""); 
	std::string args = "READ debug_handler.debug";
	bool retval;
	int t=cli.deadline;
	
	do {
		retval = this->send_socket_cmd(args, tmp_ret);	
		if(!retval)
			return false;
		if(tmp_ret.size() <= 55) {
			sleep(1);
			if((--t) == 0) {
				break;
			} else if(ret != "" && !cli.all)
				break;
		} else {
			ret += tmp_ret;
			if(!cli.all)
				break;
		}
		tmp_ret = "";
	} while(!interrupted);
//	std::cout << "ret = " << ret << std::endl;
	parse_single_traceroute_info(ret);	
	return true;	
}

/*
 * Parse the data that was received
 */
void Traceroute::parse_single_traceroute_info(std::string ret) {
	char* dump = strdup(reinterpret_cast<const char*>(ret.c_str()));
	size_t num_paths = std::count(ret.begin(), ret.end(), '<'); 
	char** debug_acks = new char *[num_paths];	
	strtok(dump, "|");
	for(int i=0; i < num_paths; i++) {
		debug_acks[i] = strtok(NULL, "<");
	}

	for(int i=0; i < num_paths; i++) {
		Route new_route(debug_acks[i], cli.dst_ip);
		routes.push_back(new_route);
	}

	delete [] debug_acks;
}

/*
 * If all castor nodes return their paths there will be some duplicates that has to removed
 */
void Traceroute::merge_routes() {
	size_t num_routes = routes.size();
	int i, j;
	Route* route1;
	Route* route2;

	for(i=num_routes-1; i >= 0; i--) {
		for(j=num_routes-1; j >= 0 && j != i; j--) {
			route1 = &routes.at(i);
			route2 = &routes.at(j);
			if(route1->entries.size() < route2->entries.size()) {
				if(route2->merge(*route1)) {
					routes.erase(routes.begin()+i);	
					break;
				}
			} else if(route1->entries.size() > route2->entries.size()) {
				if(route1->merge(*route2)) {
					routes.erase(routes.begin()+j);	
					break;
				}
			}
		}	
		num_routes = routes.size();
	}
}

/*
 * Sorts the routes in order of the rtt
 */
void Traceroute::sort_routes() {
	size_t num_routes = routes.size();
	int i, j, m;
	float rtt1, rtt2;

	if(num_routes == 1 || cli.st == ST_NORMAL)
		return;

	for(i=num_routes-1; i >= 0; i--) {
		m = 0;
		for(j=1; j <= i; j++) {
			rtt1 = routes.at(m).get_response_time();
			rtt2 = routes.at(j).get_response_time();

			if(cli.st == ST_UP) {
				if(rtt1 <= rtt2)
					m = j;
			} else  {
				if(rtt1 >= rtt2)
					m = j;
			}
		}	

		iter_swap(routes.begin() + i, routes.begin() + m);
	}
}

/*
 * Analyzes the collected data and printd it on the screen
 */
void Traceroute::analyze() {
	int i, num_entries, packet_size;
	float response_time;
	size_t num_routes = routes.size();
	std::string routes_str("");
	Route *route;

	for(i=0; i < num_routes; i++) {
		route = &routes.at(i);	
		if(cli.rt == RT_DST) {
			if(route->contains_dst) {
				setup_print_route(routes_str, route);
			}
		} else if(cli.rt == RT_ALL) {
				setup_print_route(routes_str, route);
		} else if(cli.rt == RT_NODST) {
			if(!route->contains_dst) {
				setup_print_route(routes_str, route);
			}
		}
	}	
	routes_str += "\n==============================================\n";
	std::cout << routes_str << std::endl;
}

/*
 * Prints one single Route with time, size and num_nodes
 */
void Traceroute::setup_print_route(std::string& routes_str, Route* route) {
	int num_entries = route->entries.size();
	int packet_size = route->get_packet_size();
	float response_time = route->get_response_time();
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2); 
	ss << "\n==============================================\n";
	ss << "time: " << response_time << "ms"
	      " | size: " << packet_size << "byte" 
	      " | nodes: " << num_entries << "\n";
	ss << "----------------------------------------------\n";
	ss << route->to_string(cli.at);
	routes_str += ss.str();
}

void exit_handler(int s) {
	interrupted = true;
}
Traceroute* traceroute;

int main(int argc, char** argv) {
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = exit_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	traceroute = new Traceroute(argc, argv);
	delete traceroute;
}

