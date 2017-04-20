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

#include <click/config.h>
#include <click/args.hh>
#include "castor_debug_authenticate_ack.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorDebugAuthenticateAck::configure(Vector<String>& conf, ErrorHandler* errh) {
	return Args(conf, this, errh)
		.read_mp("History", ElementCastArg("CastorHistory"), history)
		.read_mp("ID", myMac)
		.complete();
}

/*
 * If the Mac address of this node is inside the ACK return true, otherwise false
 */
bool CastorDebugAuthenticateAck::already_forwarded(const PathElem* path, uint8_t path_len) {
	int i;
	for(i=0; i < path_len; i++) {
		if(path[i].mac == myMac)
			return true;
	}
	return false;
}

/*
 * Checks if the ACK should be forwarded.
 */
void CastorDebugAuthenticateAck::push(int, Packet* p) {
	const CastorAck& ack = *reinterpret_cast<const CastorAck*>(p->data());
	const PacketId& pid = ack.auth;
	const NeighborId& src = CastorAnno::src_id_anno(p);
	const NeighborId& routedTo = history->routedTo(pid);
	int port = 0;

	// Check if this node ever forwarded the corresponding PKT 
	if(!history->hasPkt(pid)) 
		port = 1; 
	// Check whether ACK sender is equal to PKT forwarding decision
	else if(routedTo != src && routedTo != NeighborId::make_broadcast())
		port = 1;
	// Check whether we are already part of forwarder list (path)
	else if(already_forwarded(ack.path(), ack.path_len))
		port = 1;

	output(port).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDebugAuthenticateAck);
