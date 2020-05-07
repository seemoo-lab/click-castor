/*
 * Copyright (c) 2018 Milan Stute
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

#ifndef CLICK_NEIGHBOR_FILTER_HH
#define CLICK_NEIGHBOR_FILTER_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/list.hh>
#include <click/timer.hh>
#include <click/timestamp.hh>
#include "../neighbor_id.hh"

CLICK_DECLS

class NeighborFilter: public Element {
public:
	inline NeighborFilter() : active(false) {}

	const char *class_name() const { return "NeighborFilter"; }
	const char *port_count() const { return PORTS_1_1X2; }
	const char *processing() const { return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet *simple_action(Packet *);

	void add_handlers();

private:

	HashTable<EtherAddress, EtherAddress> neighbors;

	bool active;

	static int add_neighbor(const String&, Element*, void*, ErrorHandler*);
	static int clear_neighbors(const String&, Element*, void*, ErrorHandler*);
	static String read_neighbors(Element*, void*);
};

CLICK_ENDDECLS

#endif
