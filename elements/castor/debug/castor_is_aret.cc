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
