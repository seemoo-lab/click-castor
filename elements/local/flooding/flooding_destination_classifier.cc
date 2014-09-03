#include <click/config.h>
#include <click/confparse.hh>
#include <click/vector.hh>
#include "flooding_destination_classifier.hh"


CLICK_DECLS

FloodingDestinationClassifier::FloodingDestinationClassifier() {
}

FloodingDestinationClassifier::~FloodingDestinationClassifier() {
}

int FloodingDestinationClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
		"ADDR", cpkP+cpkM, cpIPAddress, &myAddr,
		"MAP", cpkP+cpkM, cpElementCast, "CastorXcastDestinationMap", &map,
		cpEnd);
}

void FloodingDestinationClassifier::push(int, Packet *p) {

	IPAddress dst(p->ip_header()->ip_dst);

	bool isDestination = false;
	const Vector<IPAddress>& dsts = map->getDestinations(dst);
	for (Vector<IPAddress>::const_iterator it = dsts.begin(); it != dsts.end(); it++) {
		if (*it == myAddr) {
			isDestination = true;
			break;
		}
	}

	if (isDestination) {
		output(0).push(p->clone()); // Deliver to host
	}

	p->set_dst_ip_anno(IPAddress::make_broadcast());
	output(1).push(p); // Forward packet

}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingDestinationClassifier)
