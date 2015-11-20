#include <click/config.h>
#include <click/args.hh>
#include <click/vector.hh>
#include "flooding_destination_classifier.hh"

CLICK_DECLS

int FloodingDestinationClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, errh)
			.read_mp("ID", myIP)
			.read_mp("MAP", ElementCastArg("CastorXcastDestinationMap"), map)
			.complete();
}

void FloodingDestinationClassifier::push(int, Packet *p) {

	GroupId dst(IPAddress(p->ip_header()->ip_dst).data());

	bool isDestination = false;
	const auto& dsts = map->get(dst);
	for (const auto& dst : dsts) {
		if (dst == GroupId(myIP)) {
			isDestination = true;
			break;
		}
	}

	if (isDestination) {
		output(0).push(p->clone()); // Deliver to host
	} else {
		p->set_dst_ip_anno(IPAddress::make_broadcast());
		WritablePacket* q = p->uniqueify();
		Flooding::setHopcount(q, Flooding::getHopcount(q) + 1); // hopcount++
		output(1).push(q); // Forward packet
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingDestinationClassifier)
