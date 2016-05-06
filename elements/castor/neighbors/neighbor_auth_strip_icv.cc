#include <click/config.h>
#include "neighbor_auth_strip_icv.hh"
#include "../castor.hh"

CLICK_DECLS

Packet* NeighborAuthStripICV::simple_action(Packet* p) {
	p->take(icv_BYTES);
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborAuthStripICV)
