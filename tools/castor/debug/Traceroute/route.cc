#include "route.hh"
#include <arpa/inet.h>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>

/*
 * Parses the debug_ack and sets the corresponding attributes.
 *
 * debug_ack example: 
 * |524|56|08-00-27-64-0F-53:192.168.56.102,08-00-27-CC-77-50:192.168.56.101|<
 */
Route::Route(char* debug_ack, char* dst_ip)
{
	char* dump = strdup(debug_ack);
	//strtok(dump, "|");
	char* timestamp_str = strtok(dump, "|");
	char* packet_size_str = strtok(NULL, "|");
	char* raw_route_str = strtok(NULL, "|");
	char* dump_route = strdup(raw_route_str);
	char* mac_ip_str;
	char* mac_address_str;
	char* ip_address_str;

	// Parses the path inside the debug_ack
	mac_address_str = strtok(dump_route, ":");
	while(mac_address_str){
		ip_address_str = strtok(NULL, ",");

		if(!strcmp(ip_address_str, dst_ip))
			contains_dst_flag = true;

		entries.push_back(RouteEntry(std::string(mac_address_str), std::string(ip_address_str)));
		mac_address_str = strtok(NULL, ":");
	} 
	
	rtt = atof(timestamp_str);
	packet_size = atoi(packet_size_str);

	// The response time of the route is also the time of the last entry
	entries.at(0).set_rtt(rtt);
}

/*
 * Checks if the given route is a prefix of this route and
 * take the rtts, if some are missing.	
 */
bool Route::merge(Route route) {
	size_t num_entries = route.entries.size();
	int i, diff;
	diff = entries.size() - num_entries;

	if(num_entries > entries.size())
		return false;

	if(!is_prefix(route))		
		return false;
	
	for(i=num_entries-1; i >= 0; i--) {
		if(!entries.at(i+diff).get_rtt()) 
			entries.at(i+diff).set_rtt(route.entries.at(i).get_rtt()); 
	}

	return true;
}

/*
 * Checks if the given route is a prefix of this route. 
 */
bool Route::is_prefix(Route route) {
	int i, diff;
	size_t num_entries = route.entries.size();
	std::string mac1(""); 
	std::string mac2("");

	if(num_entries > entries.size())
		return false; 

	diff = entries.size() - num_entries;

	for(i=num_entries-1; i >= 0; i--) {
		mac1 = entries.at(i+diff).get_mac_address();
		mac2 = route.entries.at(i).get_mac_address();

		if(mac1 != mac2)
			return false;	
	}

	return true;
}

/*
 * Converts the route object to a string.
 */
std::string Route::to_string(AddressType at)
{
	std::stringstream ss;	
	ss << std::fixed << std::setprecision(2);
	ss << "    ";
	int i;
	RouteEntry* entry;
	size_t num_entries = entries.size();
	std::string address("");
	std::string tmp("\n--> ");

	for(i=num_entries-1; i >= 0; i--) {
		entry = &(entries.at(i));

		switch(at) {
		case AT_MAC:
			address = entry->get_mac_address();
			break;
		case AT_MAC_IP:
			address = entry->get_ip_address() + "(" + entry->get_mac_address() + ")";	
			break;
		default:
			address = entry->get_ip_address();
		} 	

		if(i == 0)
			tmp = "\n";

		ss << address << " | " << entry->get_rtt() << "ms" << tmp;		
	}

	return ss.str();
}

float Route::get_rtt() {
	return rtt;
}

int Route::get_packet_size() {
	return packet_size;
}

bool Route::contains_dst() {
	return contains_dst_flag;
}
