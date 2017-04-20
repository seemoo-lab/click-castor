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

#include <click/config.h>
#include <click/args.hh>
#include "castor_lookup_route.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorLookupRoute::configure(Vector<String> &conf, ErrorHandler *errh) {
	Element* tmp;
	int result = Args(conf, this, errh)
			.read_mp("ROUTE_SELECTOR", tmp)
			.complete();
	selector = dynamic_cast<CastorRouteSelector*>(tmp);
	return result;
}

Packet* CastorLookupRoute::simple_action(Packet *p){
	CastorPkt& header = (CastorPkt&) *p->data();

	NeighborId nextHop = selector->select(header.fid, header.src, header.dst);

	if (nextHop.empty()) {
		checked_output_push(1, p);
		return 0;
	} else {
#ifdef DEBUG_HOPCOUNT
		header.set_hopcount(header.hopcount() + 1);
#endif
		// Set annotation for destination and push Packet to Output
		CastorAnno::dst_id_anno(p) = nextHop;
		CastorAnno::hop_id_anno(p) = nextHop;
		return p;
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorLookupRoute)
