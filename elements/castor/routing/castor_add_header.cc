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
#include "castor_add_header.hh"

CLICK_DECLS

int CastorAddHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("FLOW_MANAGER", ElementCastArg("CastorFlowManager"), flow)
			.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), flowtable)
			.read_or_set_p("FORCE_NONCE", force_nonce, false)
			.complete();
}

Packet* CastorAddHeader::simple_action(Packet *p) {
	// Extract source and destination from packet
	NodeId src(reinterpret_cast<const uint8_t*>(&(p->ip_header()->ip_src)));
	NodeId dst(reinterpret_cast<const uint8_t*>(&(p->ip_header()->ip_dst)));

	// Access the flow settings
	PacketLabel label = flow->getPacketLabel(src, dst);
	unsigned int fasize = 0;

	// Whether to include the nonce
	bool include_n = force_nonce || !flowtable->get(label.fid).acked;

	// Add Space for the new Header
	unsigned int diff = sizeof(CastorPkt) + ((include_n) ? sizeof(Nonce) : 0);
	WritablePacket *q = p->push(diff);
	if (!q)
		return 0;

	CastorPkt* header = reinterpret_cast<CastorPkt*>(q->data());
	header->type = CastorType::MERKLE_PKT;
	header->hsize = sizeof(Hash);
	header->set_fsize(label.size);
	include_n ? header->set_syn() : header->unset_syn();
	header->unset_dbg();
	header->unset_aret();
	header->unset_insp();
	header->len = htons(p->length());
#ifdef DEBUG_HOPCOUNT
	header->set_hopcount(0);
#endif
	header->set_fasize(fasize);
	header->src = src;
	header->dst = dst;

	header->fid = label.fid;
	header->pid = label.pid;
	header->kpkt = htons(label.num);
	header->icv = ICV();

	if (include_n)
		*header->n() = label.n;
	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddHeader)
