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

#include <click/config.h>
#include <click/args.hh>
#include <click/straccum.hh>
#include <click/error.hh>
#include "neighbor_filter.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int NeighborFilter::configure(Vector<String>& conf, ErrorHandler* errh) {
	return Args(conf, this, errh)
			.read_or_set_p("ACTIVE", active, false)
			.complete();
}

Packet* NeighborFilter::simple_action(Packet* p) {
	if (!active || neighbors.count(CastorAnno::src_id_anno(p)) > 0) {
		return p;
	} else {
		checked_output_push(1, p);
		return 0;
	}
}

int NeighborFilter::add_neighbor(const String &data, Element *e, void *, ErrorHandler *errh) {
	EtherAddress addr;
	if (cp_ethernet_address(data, &addr)) {
		NeighborFilter* nf = static_cast<NeighborFilter*>(e);
		nf->neighbors.set(addr, addr);
		return 0;
	} else {
		return errh->error("expected Ethernet address");
	}
}

int NeighborFilter::clear_neighbors(const String &, Element *e, void *, ErrorHandler *) {
	NeighborFilter* nb = static_cast<NeighborFilter*> (e);
	nb->neighbors.clear();
	return 0;
}

String NeighborFilter::read_neighbors(Element *e, void *) {
	NeighborFilter* nf = static_cast<NeighborFilter*> (e);
	StringAccum sa;
	for (const auto& neighbor_entry : nf->neighbors) {
		sa << neighbor_entry.first << "\n";
	}
	return String(sa.c_str());
}

void NeighborFilter::add_handlers() {
	add_data_handlers("active", Handler::f_read | Handler::f_write, &active);
	add_write_handler("add", add_neighbor);
	add_write_handler("clear", clear_neighbors);
	add_read_handler("read", read_neighbors);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborFilter)
