#include <click/config.h>
#include "castor_dec_ttl.hh"
#include "../castor.hh"

CLICK_DECLS

Packet* CastorDecTtl::simple_action(Packet* p) {
	CastorPkt* pkt = (CastorPkt*)p->data();
	uint8_t ttl = pkt->ttl() - 1;

	// discard the packet if the ttl is zero
	if(ttl == 0) {
		output(1).push(p);
		return 0;
	}
	pkt->set_ttl(ttl);

	return p;	
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDecTtl)
