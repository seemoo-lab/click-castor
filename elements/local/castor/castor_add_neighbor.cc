#include <click/config.h>
#include <click/confparse.hh>
#include "castor_add_neighbor.hh"

CLICK_DECLS

CastorAddNeighbor::CastorAddNeighbor() {
	neighbors = 0;
}

CastorAddNeighbor::~CastorAddNeighbor() {
}

int CastorAddNeighbor::configure(Vector<String>& conf, ErrorHandler* errh) {
	return cp_va_kparse(conf, this, errh,
			"CastorNeighbors", cpkP + cpkM, cpElementCast, "CastorNeighbors", &neighbors,
			cpEnd);
}

void CastorAddNeighbor::push(int, Packet* p) {
	CastorBeacon& beacon = (CastorBeacon&) *p->data();
	neighbors->addNeighbor(beacon.src);
	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddNeighbor)
