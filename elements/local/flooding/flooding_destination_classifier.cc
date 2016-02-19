#include <click/config.h>
#include <click/args.hh>
#include "flooding_destination_classifier.hh"

CLICK_DECLS

int FloodingDestinationClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ID", myIP)
			.read_mp("MAP", ElementCastArg("CastorXcastDestinationMap"), map)
			.complete();
}

void FloodingDestinationClassifier::push(int, Packet *p) {
	GroupId dst(IPAddress(p->ip_header()->ip_dst).data());
	const auto& dsts = map->get(dst);
	bool isDestination = false;
	for (const auto& dst : dsts) {
		if (dst == GroupId(myIP)) {
			isDestination = true;
			break;
		}
	}
	WritablePacket* q = p->uniqueify();
	Flooding::hopcount(q)++;

	int group_size = dsts.size();
	assert(group_size > 0);

	if (isDestination && group_size == 1) {
		// only deliver to host
		output(0).push(q);
	} else if (!isDestination) {
		// only forward
		output(1).push(q);
	} else { /* isDestination && group_size > 1 */
		// deliver to host AND forward
		output(0).push(q->clone());
		output(1).push(q);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingDestinationClassifier)
