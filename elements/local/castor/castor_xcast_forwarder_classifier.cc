#include <click/config.h>
#include <click/args.hh>
#include "castor_xcast_forwarder_classifier.hh"

CLICK_DECLS

int CastorXcastForwarderClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("NodeId", my_id)
			.complete();
}

void CastorXcastForwarderClassifier::push(int, Packet *p) {

	CastorXcastPkt pkt = CastorXcastPkt(p);

	// Get responsible destinations
	Vector<unsigned int> destinations;
	for (unsigned int i = 0, dstPos = 0; i < pkt.nnexthop(); dstPos += pkt.nexthop_assign(i), i++) {
		if (pkt.nexthop(i) == NeighborId::make_broadcast()
				|| pkt.nexthop(i) == my_id) {
			pkt.nexthop_assigned_dsts(i, destinations);
		}
	}

	if(destinations.empty()) {

		output(1).push(pkt.getPacket()); // Node is not in the forwarder list -> discard

	} else {
		// Node is in the forwarder list
		HashTable<uint8_t, uint8_t> toRemain;
		for(int i = 0; i < destinations.size(); i++) {
			toRemain.set(destinations[i], destinations[i]);
		}

		pkt.keep(toRemain);
		pkt.set_single_nexthop(my_id);

		output(0).push(pkt.getPacket());
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastForwarderClassifier)
