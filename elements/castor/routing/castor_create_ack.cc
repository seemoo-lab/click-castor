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
#include "castor_create_ack.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorCreateAck::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
		.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), flowtable)
		.complete();
}

Packet* CastorCreateAck::simple_action(Packet* p) {
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());

	CastorAck ack;
	ack.type = CastorType::MERKLE_ACK;
	ack.hsize = sizeof(AckAuth);
	ack.len = htons(sizeof(CastorAck));
	ack.fid  = pkt.fid;
	ack.auth = flowtable->get(pkt.fid).aauths[ntohs(pkt.kpkt)];

	WritablePacket* q = Packet::make(&ack, sizeof(CastorAck));
	CastorAnno::dst_id_anno(q) = CastorAnno::src_id_anno(p); // Set DST_ANNO to source of PKT
	CastorAnno::hop_id_anno(q) = CastorAnno::dst_id_anno(q);

	output(1).push(q); // ACK -> output 1
	return p;          // PKT -> output 0
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCreateAck)
