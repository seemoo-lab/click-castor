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
#include "castor_insert_path.hh"
#include "../castor.hh"
#include "../castor_anno.hh"


CLICK_DECLS

int CastorInsertPath::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("NodeId", myIp)
			.read_mp("ID", myMac) 
			.complete();
}

/*
 * If the INSP flag is set, all forwarding nodes insert their ip and mac
 * into the ACK.
 */
Packet* CastorInsertPath::simple_action(Packet* p) {
	const CastorAck& ack = *reinterpret_cast<const CastorAck*>(p->data());

	CastorAck header = ack;
	// The length of the packet gets larger
	header.len = htons(ntohs(header.len) + (unsigned int)sizeof(PathElem));

	// Resize the ACK, so a new PathElem(IP, MAC) can be inserted
	WritablePacket* q = p->uniqueify();
	q = q->put(sizeof(PathElem));

	// Copy the data from the old packet into the new packet
	memcpy(q->data(), &header, sizeof(CastorAck));

	// Create a new ACK with the data of the old ACK
	CastorAck& newAck = *reinterpret_cast<CastorAck*>(q->data());
	*newAck.path() = *ack.path();

	//click_chatter("insert path: mac= %s ip=%s\n", myMac.unparse().c_str(), myIp.unparse().c_str());
	// Add the IP and the MAC of this node to the ACK
	newAck.add_to_path(myMac, myIp);

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorInsertPath);
