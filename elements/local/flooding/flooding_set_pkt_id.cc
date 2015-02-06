#include <click/config.h>
#include <click/confparse.hh>
#include <click/vector.hh>
#include "flooding_set_pkt_id.hh"
#include "flooding.hh"

CLICK_DECLS

void FloodingSetPktId::push(int, Packet *p) {
	WritablePacket* q = p->uniqueify();
	Flooding::setId(q, seq);
	seq++;
	Flooding::setHopcount(q, 0);

	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingSetPktId)
