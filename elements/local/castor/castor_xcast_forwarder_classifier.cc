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

	CastorXcastPkt header = CastorXcastPkt(p);

	// Get responsible destinations
	Vector<unsigned int> destinations;
	for (unsigned int i = 0, dstPos = 0; i < header.getNNextHops(); dstPos += header.getNextHopNAssign(i), i++) {
		if (header.getNextHop(i) == IPAddress::make_broadcast()
				|| header.getNextHop(i) == myAddr) {
			header.getNextHopDestintaions(i, destinations);
		}
	}

	if(destinations.empty()) {
		output(1).push(p); // Node is not in the forwarder list -> discard
		return;
	}

	// Cleanup destination and pid lists
	Vector<PacketId> pids;
	for(int i = 0; i < destinations.size(); i++) {
		unsigned int dst = destinations[i];
		header.setDestination(header.getDestination(dst), i);
		pids.push_back(header.getPid(dst)); // Store corresponding pid
	}
	header.setNDestinations(destinations.size());
	for(int i = 0; i < pids.size(); i++) {
		header.setPid(pids[i], i);
	}

	// Set local node as single forwarder
	header.setSingleNextHop(myAddr);

	output(0).push(p); // Node is in the forwarder list
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastForwarderClassifier)
