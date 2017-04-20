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

#ifndef ROUTE_HH
#define ROUTE_HH

#include <vector>
#include "cli.hh"
#include "route_entry.hh"
#include <string>

class Route {
public:
	// Parses the debug_ack and sets the corresponding attributes
	Route(char* debug_ack, char* dst_ip);
	~Route(){ };

	// Checks if the given route is a prefix of this route and
	// take the rtts, if some are missing
	bool merge(Route& route);

	// Converts the route object to a string
	std::string to_string(AddressType at);

	// Contains all route entries, each with a rtt, mac and ip
	std::vector<RouteEntry> entries;


	bool contains_dst();
	float get_rtt();
	int get_packet_size();
private:
	// Checks if the given route is a prefix of this route 
	bool is_prefix(Route& route);

	// Is set to true, if this route contains a destinatin node
	bool contains_dst_flag = false;

	// Round trip time
	float rtt;

	// Size of the PKT that was send
	int packet_size;
};

#endif
