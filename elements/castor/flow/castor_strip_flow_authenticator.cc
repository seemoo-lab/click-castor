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
#include "castor_strip_flow_authenticator.hh"
#include "../castor.hh"

CLICK_DECLS

Packet* CastorStripFlowAuthenticator::simple_action(Packet *p) {
	CastorPkt pkt = *reinterpret_cast<const CastorPkt*>(p->data());
	Nonce n = pkt.syn() ? *reinterpret_cast<const CastorPkt*>(p->data())->n() : Nonce();

	if (pkt.fasize() == 0)
		return p;

	WritablePacket* q = p->uniqueify();

	// remove flow authenticator
	unsigned int diff = pkt.fasize() * pkt.hsize;
	q->pull(diff);

	// update length fields
	pkt.len = htons(ntohs(pkt.len) - diff);
	pkt.set_fasize(0);

	// write back header to Packet
	memcpy(q->data(), &pkt, sizeof(pkt));
	if (pkt.syn())
		*reinterpret_cast<CastorPkt*>(q->data())->n() = n;

	return q;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorStripFlowAuthenticator)
