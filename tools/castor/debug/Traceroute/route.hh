#ifndef ROUTE_HH
#define ROUTE_HH

#include <vector>
#include "route_entry.hh"
#include <string>

enum AddressType { AT_IP, AT_MAC, AT_MAC_IP };
enum RouteType { RT_DST, RT_NODST, RT_ALL }; 

class Route {
public:
	Route(char* debug_ack, char* dst_ip);
	~Route(){ };
	bool merge(Route route);
	std::string to_string(AddressType at);
	std::vector<RouteEntry> entries;
	bool contains_dst = false;
	float get_response_time();
	int get_packet_size();
private:
	bool is_prefix(Route route);
	float response_time;
	int packet_size;
};

#endif
