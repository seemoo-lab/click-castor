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
#include "castor_reconstruct_flow.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorReconstructFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Flows", ElementCastArg("CastorFlowTable"), flowtable)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorReconstructFlow::simple_action(Packet *p) {
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());

	assert(pkt.syn());

	CastorFlowEntry& e = flowtable->get(pkt.fid);

	if (e.complete()) {
		return p;
	}

	unsigned int size = 1u << pkt.fsize();
	e.aauths = new Hash[size];
	e.pids =   new Hash[size];
	Buffer<32> key(crypto->getSharedKey(pkt.src)->data());
	// Generate aauths from n
	crypto->stream(e.aauths->data(), size * pkt.hsize, pkt.n()->data(), key.data());
	for (unsigned int i = 0; i < size; i++) {
		crypto->hash(e.pids[i], e.aauths[i]);
	}
	e.set_tree(new MerkleTree(e.pids, size, *crypto));

	return p;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorReconstructFlow)
