#include <click/config.h>
#include "castor_is_sync.hh"
#include "castor.hh"

CLICK_DECLS

Packet* CastorIsSync::simple_action(Packet* p) {
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());
	if (pkt.syn) {
		output(1).push(p);
		return 0;
	}
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorIsSync)
