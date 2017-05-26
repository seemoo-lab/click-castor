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

#ifndef CLICK_CASTOR_ROUTING_TABLE_HH
#define CLICK_CASTOR_ROUTING_TABLE_HH

#include <click/element.hh>
#include "../castor.hh"
#include "../neighbor_id.hh"
#include "castor_estimator.hh"
#include "../util/ephemeral_map.hh"

CLICK_DECLS

class CastorRoutingTable : public Element {
public:
	typedef HashTable<NeighborId, CastorEstimator> FlowEntry;
	typedef ephemeral_map<FlowId, FlowEntry>::size_type size_type;

public:
	CastorRoutingTable() : default_entry(CastorEstimator(0)) {};
	~CastorRoutingTable() {
		delete flows;
	}

	const char *class_name() const { return "CastorRoutingTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	FlowEntry& at(const Hash &flow) { return flows->at(flow); };
	void insert(const Hash &flow, const FlowEntry &entry);
	size_type count(const Hash &flow) const;

	String unparse(const FlowId&) const;

	void add_handlers();

private:
	void run_timer(Timer *timer) { flows->run_timer(timer); }

	ephemeral_map<FlowId, FlowEntry> *flows;

	double updateDelta;

	FlowEntry default_entry;


	void print(const FlowId&) const;

	static String read_table_handler(Element *e, void *);
};

CLICK_ENDDECLS

#endif
