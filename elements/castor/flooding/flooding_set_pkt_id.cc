#include <click/config.h>
#include "flooding_set_pkt_id.hh"
#include "flooding.hh"

CLICK_DECLS

Packet* FloodingSetPktId::simple_action(Packet *p) {
	WritablePacket* q = p->uniqueify();
	Flooding::id(q) = seq;
	seq++;
	Flooding::hopcount(q) = 0;

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingSetPktId)
