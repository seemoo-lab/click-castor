#include <click/config.h>
#include "castor_is_insp.hh"
#include "../castor.hh"

CLICK_DECLS

/*
 * Sends the incoming packet (PKT or ACK) to output 1 if the INSP flag is set
 */
Packet* CastorIsInsp::simple_action(Packet* p) {
	if (CastorPacket::getType(p) == CastorType::PKT) {
		const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());
		if (pkt.insp()) {
			output(1).push(p);
			return 0;
		}
	}
	else if (CastorPacket::getType(p) == CastorType::ACK) {
		const CastorAck& ack = *reinterpret_cast<const CastorAck*>(p->data());
		if (ack.insp()) {
			output(1).push(p);
			return 0;
		}
	}
		
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorIsInsp)
