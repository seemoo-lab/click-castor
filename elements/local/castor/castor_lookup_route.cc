#include <click/config.h>
#include <click/args.hh>
#include "castor_lookup_route.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorLookupRoute::configure(Vector<String> &conf, ErrorHandler *errh) {
	Element* tmp;
	int result = Args(conf, this, errh)
			.read_mp("ROUTE_SELECTOR", tmp)
			.complete();
	selector = dynamic_cast<CastorRouteSelector*>(tmp);
	return result;
}

Packet* CastorLookupRoute::simple_action(Packet *p){
	CastorPkt& header = (CastorPkt&) *p->data();

	NeighborId nextHop = selector->select(header.fid, header.src, header.dst);

	if (nextHop.empty()) {
		checked_output_push(1, p);
		return 0;
	} else {
#ifdef DEBUG_HOPCOUNT
		header.hopcount++;
#endif
		// Set annotation for destination and push Packet to Output
		CastorAnno::dst_id_anno(p) = nextHop;
		CastorAnno::hop_id_anno(p) = nextHop;
		return p;
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorLookupRoute)
