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
#include "neighbor_auth_add_icv.hh"
#include "../castor_anno.hh"
#include "../castor.hh"
#include "forwarder_list.hh"

CLICK_DECLS

int NeighborAuthAddICV::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Neighbors", ElementCastArg("Neighbors"), neighbors)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.read_or_set_p("ENABLE", enable, true)
			.complete();
}

Packet* NeighborAuthAddICV::simple_action(Packet *p) {
	if (!enable)
		return p;

	WritablePacket* q = p->uniqueify();
	if (!q)
		return 0;

	ForwarderList& fl = *reinterpret_cast<ForwarderList*>(q->data());

	/*
	 * If the packet is indented for multiple receivers, we need to append a proper
	 * ICV for every such receiver.
	 */
	// count number of effective neighbors (excluding sending node)
	Vector<NeighborId> forwarders;

	if (fl.node == NeighborId::make_broadcast()) {
		for (const NeighborId& neighbor : *neighbors)
			//if (neighbor != CastorAnno::src_id_anno(p))
			forwarders.push_back(neighbor);
		if (forwarders.size() > 0)
			CastorAnno::hop_id_anno(p) = forwarders[0];
		//if (forwarders.size() == 0)
		//	click_chatter("No other node than PKT sender to forward packet to (neighbors: %u), discard ...", neighbors->size());
	} else {
		//if (fl.node != CastorAnno::src_id_anno(q))
		forwarders.push_back(fl.node);
		//else
		//	click_chatter("Trying to forward to PKT sender, discard ...");
	}

	if (forwarders.size() == 0) {
		checked_output_push(1, q);
		return 0;
	}

	// we need to set number of appended ICVs before calculating ICVs
	fl.nicv = forwarders.size();
	unsigned int trailer_size = forwarders.size() * icv_BYTES;

	q = q->put(trailer_size);
	if (!q)
		return 0;

	// calculate ICV for each neighbor
	unsigned int count = 0;
	for (const NeighborId& neighbor : forwarders) {
		if (const SymmetricKey* key = crypto->getSharedKey(neighbor)) {
			crypto->auth(q->end_data() - trailer_size + count * icv_BYTES, q->data(), q->length() - trailer_size, key->data());
			++count;
		} else {
			click_chatter("Could not find shared key for node %s. Discarding packet ...", neighbor.unparse().c_str());
			checked_output_push(1, q);
			return 0;
		}
	}

	return q;

}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborAuthAddICV)
