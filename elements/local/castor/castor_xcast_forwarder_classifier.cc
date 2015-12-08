#include <click/config.h>
#include <click/args.hh>
#include "castor_xcast_forwarder_classifier.hh"

CLICK_DECLS

int CastorXcastForwarderClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("NodeId", my_id)
			.complete();
}

void CastorXcastForwarderClassifier::push(int, Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p);

	// Get responsible destinations
	Vector<uint8_t> dsts;
	for (uint8_t hop_i = 0, dst_i = 0; hop_i < pkt.nnexthop(); dst_i += pkt.nexthop_assign(hop_i), hop_i++)
		if (pkt.nexthop(hop_i) == NeighborId::make_broadcast() ||
			pkt.nexthop(hop_i) == my_id)
			pkt.nexthop_assigned_dsts(hop_i, dsts);

	if(dsts.empty()) {
		checked_output_push(1, pkt.getPacket());
	} else {
		HashTable<uint8_t, uint8_t> dst_keep;
		for(int i = 0; i < dsts.size(); i++)
			dst_keep.set(dsts[i], dsts[i]);
		CastorXcastPkt pkt(p->uniqueify());
		pkt.keep(dst_keep);
		pkt.set_single_nexthop(my_id);
		output(0).push(pkt.getPacket());
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastForwarderClassifier)
