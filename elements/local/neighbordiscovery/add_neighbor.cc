#include <click/config.h>
#include <click/confparse.hh>
#include "add_neighbor.hh"

CLICK_DECLS

int AddNeighbor::configure(Vector<String>& conf, ErrorHandler* errh) {
	return cp_va_kparse(conf, this, errh,
			"Neighbors", cpkP + cpkM, cpElementCast, "Neighbors", &neighbors,
			cpEnd);
}

void AddNeighbor::push(int, Packet* p) {
	NeighborBeacon& beacon = (NeighborBeacon&) *p->data();
	neighbors->addNeighbor(beacon.src);
	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AddNeighbor)
