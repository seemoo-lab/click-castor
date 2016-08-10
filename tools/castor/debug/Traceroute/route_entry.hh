#ifndef ROUTE_ENTRY_HH
#define ROUTE_ENTRY_HH
#include <string>


class RouteEntry {
public:
	RouteEntry(std::string mac_address, std::string ip_address);
	~RouteEntry(){ };
	float get_response_time();
	std::string get_mac_address();
	std::string get_ip_address();
	void set_response_time(float response_time);
	void set_mac_address(std::string mac_address);
	void set_ip_address(std::string ip_address);
private:
	float response_time;
	std::string mac_address;
	std::string ip_address;
};

#endif
