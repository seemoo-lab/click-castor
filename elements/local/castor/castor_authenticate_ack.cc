/*
 * castor_validateACK.cc
 *
 *  Created on: Jun 23, 2014
 *      Author: milan
 */
#include <click/config.h>
#include <click/confparse.hh>
#include "castor_authenticate_ack.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorAuthenticateAck::configure(Vector<String>& conf, ErrorHandler* errh) {
	String arg;
    int result = cp_va_kparse(conf, this, errh,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
		"Version", cpkP, cpArgument, &arg,
        cpEnd);
	cp_integer(arg, 10, &version);
	return result;
}

void CastorAuthenticateAck::push(int, Packet* p) {

	const PacketId& pid = CastorAnno::hash_anno(p);
	const NeighborId& src = CastorAnno::src_id_anno(p);
	int port = 0;

	if (!history->hasPkt(pid)) {
		port = 1; // never forwarded corresponding PKT -> discard
	} else if (history->hasAckFrom(pid, src)) {
		port = 2; // already received ACK from this neighbor -> discard
	} else if (version <= 1) {
		const NeighborId& routedTo = history->routedTo(pid);
		if (routedTo != src && routedTo != NeighborId::make_broadcast()) {
			port = 3; // received ACK from a neighbor we never forwarded the PKT to -> discard (standard Castor)
		}
	} else {
#if 1
		const NeighborId& routedTo = history->routedTo(pid);
		if (routedTo != src && routedTo != NeighborId::make_broadcast()) {
			port = 3; // received ACK from a neighbor we never forwarded the PKT to -> discard (standard Castor)
		}
#elif 0
		// Check whether the ACK was received from one of our PKT senders
		if (history->routedTo(pid) != NodeId::make_broadcast()) {
			for (size_t i = 1; i < history->getPkts(pid); i++) {
				const NodeId pktSender = history->getPktSenders(pid)[i];
				if (src == pktSender) {
					port = 4; // received ACK from a neighbor that has forwarded us the PKT -> discard (Castor improvement)
					break;
				}
			}
		}
#endif
		const NeighborId& firstPktSender = history->getPktSenders(pid)[0];
		if (src == firstPktSender) {
			port = 4; // received ACK from the neighbor that initially forwarded us the PKT -> discard (Castor improvement)
		}
	}

	output(port).push(p);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAuthenticateAck)
