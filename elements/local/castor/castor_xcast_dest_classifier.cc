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

	CastorXcastPkt pkt = CastorXcastPkt(p);

	bool delivered = false;
	bool forwarded = false;

	unsigned int nDests = pkt.getNDestinations();

	for (unsigned int i = 0; i < nDests; i++)
		if (my_end_node_id == pkt.getDestination(i)) {
			delivered = true;

			CastorXcastPkt localPkt = CastorXcastPkt(pkt.getPacket()->clone()->uniqueify());

			// Cleanup PKT header
			localPkt.setSingleDestination(i);
			localPkt.setSingleNextHop(my_id);

			output(0).push(localPkt.getPacket()); // local node is destination
			break;
		}

	if (nDests > (delivered ? 1 : 0)) {
		forwarded = true;

		// If packet was delivered, remove own address from destination list
		if(delivered) {
			pkt.removeDestination(my_end_node_id);
			pkt.setSingleNextHop(my_id);
		}

		output(1).push(pkt.getPacket());
	} else {
		// We delivered a copy
		pkt.getPacket()->kill();
	}

	assert(delivered || forwarded);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastDestClassifier)
