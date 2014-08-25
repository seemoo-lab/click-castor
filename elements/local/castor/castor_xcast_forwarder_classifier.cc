#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_xcast_forwarder_classifier.hh"

CLICK_DECLS

CastorXcastForwarderClassifier::CastorXcastForwarderClassifier() {
}

CastorXcastForwarderClassifier::~CastorXcastForwarderClassifier() {
}

int CastorXcastForwarderClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
			.read_mp("ADDR", myAddr)
			.complete() < 0)
		return -1;
	return 0;
}

void CastorXcastForwarderClassifier::push(int, Packet *p) {

	CastorXcastPkt pkt = CastorXcastPkt(p);

	// Get responsible destinations
	Vector<unsigned int> destinations;
	for (unsigned int i = 0, dstPos = 0; i < pkt.getNNextHops(); dstPos += pkt.getNextHopNAssign(i), i++) {
		if (pkt.getNextHop(i) == IPAddress::make_broadcast()
				|| pkt.getNextHop(i) == myAddr) {
			pkt.getNextHopDestintaions(i, destinations);
		}
	}

	if(destinations.empty()) {
//		// FIXME if we are destination -> deliver
//		for (unsigned int i = 0; i < pkt.getNDestinations(); i++)
//			if (pkt.getDestination(i) == myAddr) {
//				pkt.setSingleDestination(i);
//				pkt.setSingleNextHop(myAddr);
//				output(0).push(pkt.getPacket());
//				return;
//			}
		output(1).push(pkt.getPacket()); // Node is not in the forwarder list -> discard
		return;
	}

	HashTable<uint8_t, uint8_t> toRemain;
	for(int i = 0; i < destinations.size(); i++) {
		toRemain.set(destinations[i], destinations[i]);
	}

	pkt.keepDestinations(toRemain);
	pkt.setSingleNextHop(myAddr);

	output(0).push(pkt.getPacket()); // Node is in the forwarder list
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastForwarderClassifier)
