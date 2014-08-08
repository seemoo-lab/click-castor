#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "castor_xcast_lookup_route.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastLookupRoute::CastorXcastLookupRoute() {
}

CastorXcastLookupRoute::~CastorXcastLookupRoute() {
}

int CastorXcastLookupRoute::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &_table,
        cpEnd);
}

void CastorXcastLookupRoute::push(int, Packet *p){
	CastorXcastPkt pkt = CastorXcastPkt(p);

	HashTable<IPAddress,Vector<unsigned int> > map;

	// Lookup routes
	for(unsigned int i = 0; i < pkt.getNDestinations(); i++) {
		IPAddress nextHop = _table->lookup(pkt.getFlowId(), pkt.getDestination(i));
		if(!map.get_pointer(nextHop))
			map.set(nextHop, Vector<unsigned int>());
		Vector<unsigned int>* entry = map.get_pointer(nextHop);
		entry->push_back(i);
	}

	// Write new routes to packet
	pkt.setNextHopMapping(map);

	// Set annotation for destination and push Packet to Output
	pkt.getPacket()->set_dst_ip_anno(IPAddress::make_broadcast()); // XXX: unicast if only one next hop

    output(0).push(pkt.getPacket());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastLookupRoute)
