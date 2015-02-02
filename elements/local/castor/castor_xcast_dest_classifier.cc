#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_xcast_dest_classifier.hh"

CLICK_DECLS

int CastorXcastDestClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
			.read_mp("ADDR", myId)
			.complete() < 0)
		return -1;
	return 0;
}

void CastorXcastDestClassifier::push(int, Packet *p) {

	CastorXcastPkt pkt = CastorXcastPkt(p);

	bool delivered = false;
	bool forwarded = false;

	unsigned int nDests = pkt.getNDestinations();

	for (unsigned int i = 0; i < nDests; i++)
		if (myId == pkt.getDestination(i)) {
			delivered = true;

			CastorXcastPkt localPkt = CastorXcastPkt(pkt.getPacket()->clone()->uniqueify());

			// Cleanup PKT header
			localPkt.setSingleDestination(i);
			localPkt.setSingleNextHop(myId);

			output(0).push(localPkt.getPacket()); // local node is destination
			break;
		}

	if (nDests > (delivered ? 1 : 0)) {
		forwarded = true;

		// If packet was delivered, remove own address from destination list
		if(delivered) {
			pkt.removeDestination(myId);
			pkt.setSingleNextHop(myId);
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
