#include "route.hh"
#include <arpa/inet.h>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>

Route::Route(char* debug_ack, char* dst_ip)
{
	char* dump = strdup(debug_ack);
	//strtok(dump, "|");
	char* timestamp_str = strtok(dump, "|");
	char* packet_size_str = strtok(NULL, "|");
	char* raw_route_str = strtok(NULL, "|");
	char* dump_route = strdup(raw_route_str);
	char* dump_mac_ip;
	char* mac_ip_str;
	char* mac_address_str;
	char* ip_address_str;



	mac_address_str = strtok(dump_route, ":");
	while(mac_address_str){
		ip_address_str = strtok(NULL, ",");
		if(!strcmp(ip_address_str, dst_ip))
			contains_dst = true;
		entries.push_back(RouteEntry(std::string(mac_address_str), std::string(ip_address_str)));
		mac_address_str = strtok(NULL, ":");
	} 
	
	response_time = atof(timestamp_str);
	packet_size = atoi(packet_size_str);

	// the response time of the route is also the time of the last entry
	entries.at(0).set_response_time(response_time);
}

bool Route::merge(Route route) {
	size_t num_entries = route.entries.size();
	int i, diff;
	diff = this->entries.size() - num_entries;
	if(num_entries > this->entries.size())
		return false;
	if(!is_prefix(route))		
		return false;
	
	for(i=num_entries-1; i >= 0; i--) {
		if(!this->entries.at(i+diff).get_response_time()) 
			this->entries.at(i+diff).set_response_time(route.entries.at(i).get_response_time()); 
	}
	return true;
}

bool Route::is_prefix(Route route) {
	int i, diff;
	size_t num_entries = route.entries.size();
	std::string mac1(""); 
	std::string mac2("");
	if(num_entries > this->entries.size())
		return false; 
	diff = entries.size() - num_entries;
	for(i=num_entries-1; i >= 0; i--) {
		mac1 = this->entries.at(i+diff).get_mac_address();
		mac2 = route.entries.at(i).get_mac_address();
		if(mac1 != mac2)
			return false;	
	}

	return true;
}

std::string Route::to_string(AddressType at)
{
	std::stringstream ss;	
	ss << std::fixed << std::setprecision(2);
	ss << "    ";
	int i;
	RouteEntry* entry;
	size_t num_entries = this->entries.size();
	std::string address("");
	std::string tmp("\n--> ");
	for(i=num_entries-1; i >= 0; i--) {
		entry = &(this->entries.at(i));
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
		ss << address << " | " << entry->get_response_time() << "ms" << tmp;		
	}
	return ss.str();
}

float Route::get_response_time() {
	return response_time;
}

int Route::get_packet_size() {
	return packet_size;
}
