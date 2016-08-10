#include <click/config.h>
#include "castor_is_dbg.hh"
#include "../castor.hh"

CLICK_DECLS

Packet* CastorIsDbg::simple_action(Packet* p) {
	if (CastorPacket::getType(p) == CastorType::PKT) {
		const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());
		if (pkt.dbg()) {
			click_chatter("isDBG (PKT)");
			output(1).push(p);
			return 0;
		}
	}
	else if (CastorPacket::getType(p) == CastorType::ACK) {
		const CastorAck& ack = *reinterpret_cast<const CastorAck*>(p->data());
		if (ack.dbg()) {
			click_chatter("isDBG (ACK)");
			output(1).push(p);
			return 0;
		}
	}
		
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorIsDbg)
