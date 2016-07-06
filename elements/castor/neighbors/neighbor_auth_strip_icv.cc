#include <click/config.h>
#include "neighbor_auth_strip_icv.hh"
#include "forwarder_list.hh"
#include "../castor.hh"

CLICK_DECLS

Packet* NeighborAuthStripICV::simple_action(Packet* p) {
	const ForwarderList& fl = *reinterpret_cast<const ForwarderList*>(p->data());
	p->take(fl.nicv * icv_BYTES);
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborAuthStripICV)
