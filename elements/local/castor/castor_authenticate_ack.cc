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

	if(!history->hasPkt(pid)) {
		output(1).push(p); // never forwarded corresponding PKT -> discard
	} else if(history->isExpired(pid)) {
		output(2).push(p); // ACK arrived too late -> discard
	} else if (history->hasAckFrom(pid, src)) {
		output(3).push(p); // already received ACK from this neighbor -> discard
	} else {
		const IPAddress firstPktSender = history->getPktSenders(pid)[0];
		if (version > 1 && (src == firstPktSender || (src != history->routedTo(pid) && history->routedTo(pid) != IPAddress::make_broadcast()))) {
			output(4).push(p); // received ACK from a neighbor to which PKT was not forwarded -> discard (Castor improvement)
		} else {
			output(0).push(p);
		}
	}

}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAuthenticateAck)
