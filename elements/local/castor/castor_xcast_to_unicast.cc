#include <click/config.h>
#include <click/confparse.hh>
#include "castor_xcast_to_unicast.hh"

CLICK_DECLS

int CastorXcastToUnicast::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorXcastDestinationMap", cpkP+cpkM, cpElementCast, "CastorXcastDestinationMap", &map,
        cpEnd);
}

void CastorXcastToUnicast::push(int, Packet *p) {
	// Extract source and destination from packet
	const GroupId& destination = reinterpret_cast<const GroupId&>(p->ip_header()->ip_dst);
	if (destination.is_multicast()) {
		const auto& destinations = map->get(destination);
		for (int i = 0; i < destinations.size(); i++) {
			WritablePacket* q;
			if (i < destinations.size() - 1)
				q = p->clone()->uniqueify();
			else
				q = p->uniqueify();
			reinterpret_cast<NodeId&>(q->ip_header()->ip_dst) = destinations[i];
			output(1).push(q);
		}
	} else {
		// Unchanged
		output(0).push(p);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastToUnicast)
