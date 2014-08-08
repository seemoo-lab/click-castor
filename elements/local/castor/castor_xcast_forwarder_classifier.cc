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
	return Args(conf, this, errh)
			.read_mp("ADDR", myAddr)
			.complete();
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
		output(1).push(pkt.getPacket()); // Node is not in the forwarder list -> discard
		return;
	}

	// Cleanup destination and pid lists
	Vector<PacketId> pids;
	for(int i = 0; i < destinations.size(); i++) {
		unsigned int dst = destinations[i];
		pkt.setDestination(pkt.getDestination(dst), i);
		pids.push_back(pkt.getPid(dst)); // Store corresponding pid
	}
	pkt.setNDestinations(destinations.size());
	for(int i = 0; i < pids.size(); i++) {
		pkt.setPid(pids[i], i);
	}

	// Set local node as single forwarder
	pkt.setSingleNextHop(myAddr);

	output(0).push(pkt.getPacket()); // Node is in the forwarder list
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastForwarderClassifier)
