/*
 * Copyright (c) 2016 Milan Schmittner
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

#ifndef CLICK_NEIGHBOR_BEACON_GENERATOR_HH
#define CLICK_NEIGHBOR_BEACON_GENERATOR_HH

#include <click/element.hh>
#include <click/timer.hh>
#include "../neighbor_id.hh"

#define ETHERTYPE_BEACON 0x88B5 // 0x88B5 and 0x88B6 reserved for private experiments, so we use them

CLICK_DECLS

class NeighborBeaconGenerator : public Element {
public:
	const char *class_name() const { return "NeighborBeaconGenerator"; }
	const char *port_count() const { return PORTS_0_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler*);

	void run_timer(Timer*);
private:
	Timer timer;
	bool enabled;
	unsigned int interval;
	NeighborId my_id;
};

CLICK_ENDDECLS

#endif
