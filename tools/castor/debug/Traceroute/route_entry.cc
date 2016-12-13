#include "route_entry.hh"

RouteEntry::RouteEntry(std::string mac, std::string ip)
{
	mac_address = mac;
	ip_address = ip;
	rtt = 0;
}

float RouteEntry::get_rtt()
{
	return rtt;
}

std::string RouteEntry::get_mac_address()
{
	return mac_address;
}

std::string RouteEntry::get_ip_address()
{
	return ip_address;
}

void RouteEntry::set_rtt(float t)
{
	rtt = t;	
}

void RouteEntry::set_mac_address(std::string mac)
{
	mac_address = mac;
}

void RouteEntry::set_ip_address(std::string ip)
{
	ip_address = ip;
}
