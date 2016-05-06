#include <click/config.h>
#include <click/args.hh>
#include "castor_set_ack_nexthop.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorSetAckNexthop::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("HISTORY", ElementCastArg("CastorHistory"), history)
			.read_mp("NEIGHBORS", ElementCastArg("Neighbors"), neighbors)
			.complete();
}

Packet* CastorSetAckNexthop::simple_action(Packet* p) {
	const PacketId& pid = CastorAnno::hash_anno(p);
	NeighborId dst = history->getPktSenders(pid)[0];  // set default ACK destination to initial PKT sender

	// Walk through the list of PKT senders and select the first one that is still a neighbor
	size_t active_count = 0;
	for (const auto& sender : history->getPktSenders(pid)) {
		if (neighbors->contains(sender)) {
			if (active_count == 0)
				dst = sender;
			active_count++;
		}
	}
	// Use broadcast if there are at least two other neighbors than the ACK sender
	// We also use broadcast (opportunistically) if the ACK sender is our only current neighbor
	bool use_broadcast = active_count != 2;

	// Set packet destination
	CastorAnno::dst_id_anno(p) = use_broadcast ? NeighborId::make_broadcast() : dst;
	CastorAnno::hop_id_anno(p) = dst;

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorSetAckNexthop)
