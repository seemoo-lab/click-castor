#include <click/config.h>
#include <click/args.hh>
#include "castor_xcast_dest_classifier.hh"

CLICK_DECLS

int CastorXcastDestClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ENDID", my_end_node_id)
			.read_mp("ID", my_id)
			.complete();
}

void CastorXcastDestClassifier::push(int, Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p->uniqueify());

	int my_index = pkt.dst_index(my_end_node_id);
	bool deliver = my_index > -1;
	bool forward = pkt.ndst() > (deliver ? 1 : 0);

	if (deliver && forward) {
		CastorXcastPkt pkt_copy = CastorXcastPkt(pkt.getPacket()->clone()->uniqueify());
		pkt_copy.set_single_dst(my_index);
		pkt_copy.set_single_nexthop(my_id);
		output(0).push(pkt_copy.getPacket());
		pkt.remove(my_index);
		pkt.set_single_nexthop(my_id);
		output(1).push(pkt.getPacket());
	} else if (deliver) {
		pkt.set_single_dst(my_index);
		pkt.set_single_nexthop(my_id);
		output(0).push(pkt.getPacket());
	} else if (forward) {
		pkt.set_single_nexthop(my_id);
		output(1).push(pkt.getPacket());
	} else {
		assert(false); // assert(deliver || forward)
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastDestClassifier)
