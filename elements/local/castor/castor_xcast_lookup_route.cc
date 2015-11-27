#include <click/config.h>
#include <click/confparse.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "castor_xcast_lookup_route.hh"
#include "castor_xcast.hh"
#include "castor_anno.hh"
#include "../neighbordiscovery/neighbor_id.hh"

CLICK_DECLS

int CastorXcastLookupRoute::configure(Vector<String> &conf, ErrorHandler *errh) {
	Element* tmp = 0;
    int result = cp_va_kparse(conf, this, errh,
		"CastorRouteSelector", cpkP+cpkM, cpElement, &tmp,
        cpEnd);
    // Have to cast manually; cpElementCast complains about type not matching
    selector = dynamic_cast<CastorRouteSelector*>(tmp);
    return result;
}

void CastorXcastLookupRoute::push(int, Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p);

	HashTable<NeighborId, Vector<unsigned int> > map;

	size_t nDestinations = pkt.getNDestinations();

	Vector<NodeId> allDestinations;
	for(unsigned int i = 0; i < nDestinations; i++)
		allDestinations.push_back(pkt.getDestination(i));

	// Lookup routes
	for(unsigned int i = 0; i < nDestinations; i++) {
		NeighborId nextHop = selector->select(pkt.getFlowId(), pkt.getDestination(i), &allDestinations, pkt.getPid(i));
		if(!map.get_pointer(nextHop))
			map.set(nextHop, Vector<unsigned int>());
		Vector<unsigned int>* entry = map.get_pointer(nextHop);
		entry->push_back(i);
	}

	// Write new routes to packet
	pkt.setNextHopMapping(map);

	// Set annotation for destination and push Packet to Output
	Vector<NeighborId> nexthopMac;
	nexthopMac.push_back(NeighborId::make_broadcast());
	uint8_t best = 0;
	for (uint8_t i = 0; i < pkt.getNNextHops(); i++) {
		if (pkt.getNextHop(i) != NeighborId::make_broadcast()) {
			if (pkt.getNextHopNAssign(i) > best) {
				nexthopMac.clear();
				best = pkt.getNextHopNAssign(i);
			}
			if (pkt.getNextHopNAssign(i) == best) {
				nexthopMac.push_back(pkt.getNextHop(i));
			}
		}
	}
	int randIndex = click_random() % nexthopMac.size();
	CastorAnno::hop_id_anno(pkt.getPacket()) = nexthopMac[randIndex];  // This is the address we want the MAC layer to transmit to

	// If there is only a single recipient, we unicast to his address; otherwise broadcast
	NeighborId nexthop = pkt.getNNextHops() == 1 ? pkt.getNextHop(0) : NeighborId::make_broadcast();
	CastorAnno::dst_id_anno(pkt.getPacket()) = nexthop;

#ifdef DEBUG_HOPCOUNT
	pkt.incHopcount();
#endif

	output(0).push(pkt.getPacket());

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastLookupRoute)
