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

#ifndef CLICK_CASTOR_TIMEOUT_TABLE_HH
#define CLICK_CASTOR_TIMEOUT_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../castor.hh"
#include "castor_timeout.hh"
#include "../neighbor_id.hh"
#include "../util/ephemeral_map.hh"

CLICK_DECLS

class CastorTimeoutTable : public Element {
public:
	~CastorTimeoutTable() {
		delete flows;
	}

	const char *class_name() const { return "CastorTimeoutTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	CastorTimeout& getTimeout(const FlowId& flow, const NeighborId& forwarder);
	const CastorTimeout& getTimeoutNoTouch(const FlowId& flow, const NeighborId& forwarder) const;

private:
	typedef HashTable<NeighborId, CastorTimeout> ForwarderEntry;
	ephemeral_map<FlowId, ForwarderEntry> *flows;

	void run_timer(Timer *timer) { flows->run_timer(timer); }
};

CLICK_ENDDECLS

#endif
