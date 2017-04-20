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

#ifndef ROUTE_ENTRY_HH
#define ROUTE_ENTRY_HH
#include <string>


class RouteEntry {
public:
	RouteEntry(std::string mac_address, std::string ip_address);
	~RouteEntry(){ };

	float get_rtt();
	std::string get_mac_address();
	std::string get_ip_address();

	void set_rtt(float response_time);
	void set_mac_address(std::string mac_address);
	void set_ip_address(std::string ip_address);
private:
	// Round trip time in ms 
	float rtt;

	std::string mac_address;
	std::string ip_address;
};

#endif
