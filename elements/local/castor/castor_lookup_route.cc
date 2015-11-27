#include <click/config.h>
#include <click/confparse.hh>
#include "castor_lookup_route.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorLookupRoute::configure(Vector<String> &conf, ErrorHandler *errh) {
	Element* tmp = 0;
    int result = cp_va_kparse(conf, this, errh,
		"CastorRouteSelector", cpkP+cpkM, cpElement, &tmp,
        cpEnd);
    // Have to cast manually; cpElementCast complains about type not matching
    selector = dynamic_cast<CastorRouteSelector*>(tmp);
    return result;
}

void CastorLookupRoute::push(int, Packet *p){
	CastorPkt& header = (CastorPkt&) *p->data();

	// Lookup
	NeighborId nextHop = selector->select(header.fid, header.dst, 0, header.pid);

	if (nextHop.empty())
		output(1).push(p);

	else {
#ifdef DEBUG_HOPCOUNT
		header.hopcount++;
#endif
		// Set annotation for destination and push Packet to Output
		CastorAnno::dst_id_anno(p) = nextHop;
		CastorAnno::hop_id_anno(p) = nextHop;

		output(0).push(p);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorLookupRoute)
