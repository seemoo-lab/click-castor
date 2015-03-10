#include <click/config.h>
#include <click/confparse.hh>

#include "castor_xcast_to_unicast.hh"

CLICK_DECLS

int CastorXcastToUnicast::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorXcastDestinationMap", cpkP+cpkM, cpElementCast, "CastorXcastDestinationMap", &_map,
        cpEnd);
}

void CastorXcastToUnicast::push(int, Packet *p) {
	// Extract source and destination from packet
	IPAddress destination = p->ip_header()->ip_dst.s_addr;
	if (destination.is_multicast()) {
		const Vector<IPAddress>& destinations = _map->getDestinations(destination);

		for (int i = 0; i < destinations.size(); i++) {
			WritablePacket* q = p->clone()->uniqueify();
			q->ip_header()->ip_dst = destinations[i].in_addr();
			output(1).push(q);
		}
	} else {
		output(0).push(p);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastToUnicast)
