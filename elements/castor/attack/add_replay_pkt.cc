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

#include "add_replay_pkt.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int AddReplayPkt::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("STORE", ElementCastArg("ReplayStore"), store)
			.complete();
}

Packet* AddReplayPkt::simple_action(Packet *p) {
	CastorPkt& pkt = (CastorPkt&) *p->data();
	WritablePacket* clone = p->clone()->uniqueify();
	clone->take(pkt.payload_len());
	reinterpret_cast<CastorPkt&>(*clone->data()).len = htons(pkt.header_len());
	CastorAnno::dst_id_anno(clone) = NeighborId::make_broadcast();
	CastorAnno::hop_id_anno(clone) = NeighborId::make_broadcast();
	store->add_pkt(pkt.pid, clone);
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AddReplayPkt)
