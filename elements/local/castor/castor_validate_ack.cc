/*
 * castor_validateACK.cc
 *
 *  Created on: Jun 23, 2014
 *      Author: milan
 */
#include <click/config.h>
#include <click/confparse.hh>
#include "castor_validate_ack.hh"

CLICK_DECLS

CastorValidateACK::CastorValidateACK() {
}

CastorValidateACK::~CastorValidateACK() {
}

int CastorValidateACK::configure(Vector<String>& conf, ErrorHandler* errh) {
    return cp_va_kparse(conf, this, errh,
    	"Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        cpEnd);
}

void CastorValidateACK::push(int, Packet* p) {

	const PacketId& pid = CastorPacket::getPidAnnotationFromAck(p);
	const IPAddress src = p->dst_ip_anno();

	if(!history->hasPkt(pid)) {
		output(1).push(p); // never forwarded corresponding PKT -> discard
	} else if(history->isExpired(pid)) {
		output(2).push(p); // ACK arrived too late -> discard
	} else if (history->hasAckFrom(pid, src)) {
		output(3).push(p); // already received PKT from this neighbor -> discard
	} else {
	    output(0).push(p);
	}

}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorValidateACK)
