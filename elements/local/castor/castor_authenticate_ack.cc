/*
 * castor_validateACK.cc
 *
 *  Created on: Jun 23, 2014
 *      Author: milan
 */
#include <click/config.h>
#include <click/confparse.hh>
#include "castor_authenticate_ack.hh"

CLICK_DECLS

int CastorAuthenticateAck::configure(Vector<String>& conf, ErrorHandler* errh) {
	String arg;
    int result = cp_va_kparse(conf, this, errh,
    	"Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
		"Version", cpkP, cpArgument, &arg,
        cpEnd);
	version = 1;
	cp_integer(arg, 10, &version);
	return result;
}

void CastorAuthenticateAck::push(int, Packet* p) {

	const PacketId& pid = (PacketId&) *CastorPacket::getCastorAnno(p);
	const IPAddress src = CastorPacket::src_ip_anno(p);
	int port = 0;

	if (!history->hasPkt(pid)) {
		port = 1; // never forwarded corresponding PKT -> discard
	} else if(history->isExpired(pid)) {
		port = 2; // ACK arrived too late -> discard
	} else if (history->hasAckFrom(pid, src)) {
		port = 3; // already received ACK from this neighbor -> discard
	} else if (version <= 1) {
		const IPAddress routedTo = history->routedTo(pid);
		if (routedTo != src && routedTo != IPAddress::make_broadcast()) {
			port = 4; // received ACK from a neighbor we never forwarded the PKT to -> discard (standard Castor)
		}
	} else {
#if 1
		const IPAddress routedTo = history->routedTo(pid);
		if (routedTo != src && routedTo != IPAddress::make_broadcast()) {
			port = 4; // received ACK from a neighbor we never forwarded the PKT to -> discard (standard Castor)
		}
#elif 0
		// Check whether the ACK was received from one of our PKT senders
		if (history->routedTo(pid) != IPAddress::make_broadcast()) {
			for (size_t i = 1; i < history->getPkts(pid); i++) {
				const IPAddress pktSender = history->getPktSenders(pid)[i];
				if (src == pktSender) {
					port = 5; // received ACK from a neighbor that has forwarded us the PKT -> discard (Castor improvement)
					break;
				}
			}
		}
#endif
		const IPAddress firstPktSender = history->getPktSenders(pid)[0];
		if (src == firstPktSender) {
			port = 5; // received ACK from the neighbor that initially forwarded us the PKT -> discard (Castor improvement)
		}
	}

	output(port).push(p);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAuthenticateAck)
