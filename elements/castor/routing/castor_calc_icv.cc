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
#include "castor_calc_icv.hh"
#include "../castor.hh"

CLICK_DECLS

int CastorCalcICV::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorCalcICV::simple_action(Packet *p) {
	WritablePacket* q = p->uniqueify();
	if (!q)
		return 0;
	CastorPkt& pkt = (CastorPkt&) *q->data();

	const SymmetricKey* sk = crypto->getSharedKey(pkt.dst);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt.dst.unparse().c_str());
		checked_output_push(1, q);
		return 0;
	}

	assert(pkt.icv == ICV());
	assert(pkt.fasize() == 0);
#ifdef DEBUG_HOPCOUNT
	assert(pkt.hopcount() == 0);
#endif

	crypto->auth(pkt.icv, q->data(), ntohs(pkt.len), sk->data());

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCalcICV)
