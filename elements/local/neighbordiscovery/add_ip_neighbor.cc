#include <click/config.h>
#include <click/confparse.hh>
#include "add_ip_neighbor.hh"

CLICK_DECLS

int AddIPNeighbor::configure(Vector<String>& conf, ErrorHandler* errh) {
	return cp_va_kparse(conf, this, errh,
			"Neighbors", cpkP + cpkM, cpElementCast, "Neighbors", &neighbors,
			"ENABLE", cpkP, cpBool, &enabled,
			cpEnd);
}

void AddIPNeighbor::push(int, Packet* p) {
	if (enabled && p->has_network_header()) {
		neighbors->addNeighbor(p->ip_header()->ip_src);
	}
	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AddIPNeighbor)
