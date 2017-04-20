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
#include "castor_retransmit_ack.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorRetransmitAck::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("CastorHistory", ElementCastArg("CastorHistory"), history)
			.complete();
}

void CastorRetransmitAck::push(int, Packet *p) {
	// We already forwarded a valid ACK for this PKT, so retransmit
	CastorPkt& pkt = (CastorPkt&) *p->data();

	assert(history->hasAck(pkt.pid));

	// Generate new ACK
	CastorAck ack;
	ack.type = CastorType::MERKLE_ACK;
	ack.hsize = sizeof(Hash);
	ack.len = htons(sizeof(CastorAck));
	ack.fid  = history->getFlowId(pkt.pid);
	ack.auth = history->getAckAuth(pkt.pid);
#ifdef DEBUG_ACK_SRCDST
	ack.src = pkt.dst;
	ack.dst = pkt.src;
#endif

	WritablePacket* q = Packet::make(&ack, sizeof(CastorAck));
	CastorAnno::dst_id_anno(q) = CastorAnno::src_id_anno(p); // Unicast ACK to PKT sender
	CastorAnno::hop_id_anno(q) = CastorAnno::dst_id_anno(q);
	CastorAnno::hash_anno(q)   = pkt.pid;

	assert(history->hasPktFrom(pkt.pid, CastorAnno::dst_id_anno(q)));

	// No longer need PKT
	p->kill();

	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRetransmitAck)
