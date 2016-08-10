#include "route_entry.hh"

RouteEntry::RouteEntry(std::string mac_address, std::string ip_address)
{
	this->mac_address = mac_address;
	this->ip_address = ip_address;
	this->response_time = 0;
}

float RouteEntry::get_response_time()
{
	return this->response_time;
}

std::string RouteEntry::get_mac_address()
{
	return this->mac_address;
}

std::string RouteEntry::get_ip_address()
{
	return this->ip_address;
}

void RouteEntry::set_response_time(float response_time)
{
	this->response_time = response_time;	
}

void RouteEntry::set_mac_address(std::string mac_address)
{
	this->mac_address = mac_address;
}

void RouteEntry::set_ip_address(std::string ip_address)
{
	this->ip_address = ip_address;
}
