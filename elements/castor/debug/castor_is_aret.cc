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
#include "castor_is_aret.hh"
#include "../castor.hh"

CLICK_DECLS

/*
 * Sends the incoming PKT to output 1 if the ARET flag is set
 */
Packet* CastorIsAret::simple_action(Packet* p) {
	if (CastorPacket::getType(p) == CastorType::PKT) {
		CastorPkt& pkt = (CastorPkt&) *p->data();
		if (pkt.aret()) {
			output(1).push(p);
			return 0;
		}
	}
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorIsAret)
