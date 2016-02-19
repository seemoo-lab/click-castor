#include <click/config.h>
#include <click/args.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "castor_xcast_lookup_route.hh"
#include "castor_xcast.hh"
#include "castor_anno.hh"
#include "../neighbordiscovery/neighbor_id.hh"

CLICK_DECLS

int CastorXcastLookupRoute::configure(Vector<String> &conf, ErrorHandler *errh) {
	Element* tmp;
    int result = Args(conf, this, errh)
    		.read_mp("ROUTE_SELECTOR", tmp)
			.complete();
    selector = dynamic_cast<CastorRouteSelector*>(tmp);
    return result;
}

void CastorXcastLookupRoute::push(int, Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p);

	HashTable<NeighborId, Vector<unsigned int> > map;

	size_t nDestinations = pkt.ndst();

	Vector<NodeId> allDestinations;
	for(unsigned int i = 0; i < nDestinations; i++)
		allDestinations.push_back(pkt.dst(i));

	// Lookup routes
	for(unsigned int i = 0; i < nDestinations; i++) {
		NeighborId nextHop = selector->select(pkt.fid(), pkt.dst(i), &allDestinations, pkt.pid(i));
		map[nextHop].push_back(i);
	}

	// Write new routes to packet
	pkt.set_nexthop_assign(map);

	// Set annotation for destination and push Packet to Output
	Vector<NeighborId> nexthopMac;
	nexthopMac.push_back(NeighborId::make_broadcast());
	uint8_t best = 0;
	for (uint8_t i = 0; i < pkt.nnexthop(); i++) {
		if (pkt.nexthop(i) != NeighborId::make_broadcast()) {
			if (pkt.nexthop_assign(i) > best) {
				nexthopMac.clear();
				best = pkt.nexthop_assign(i);
			}
			if (pkt.nexthop_assign(i) == best) {
				nexthopMac.push_back(pkt.nexthop(i));
			}
		}
	}
	int randIndex = click_random() % nexthopMac.size();
	CastorAnno::hop_id_anno(pkt.getPacket()) = nexthopMac[randIndex];  // This is the address we want the MAC layer to transmit to

	// If there is only a single recipient, we unicast to his address; otherwise broadcast
	NeighborId nexthop = pkt.nnexthop() == 1 ? pkt.nexthop(0) : NeighborId::make_broadcast();
	CastorAnno::dst_id_anno(pkt.getPacket()) = nexthop;

#ifdef DEBUG_HOPCOUNT
	pkt.hopcount()++;
#endif

	output(0).push(pkt.getPacket());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastLookupRoute)
