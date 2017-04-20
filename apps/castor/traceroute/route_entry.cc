/*
 * Copyright (c) 2016 Simon Schmitt
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

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
