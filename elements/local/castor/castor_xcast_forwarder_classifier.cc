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

	for (unsigned int i = 0; i < header.getNDestinations(); i++)
		if (header.getDestination(i) == IPAddress::make_broadcast() //
				|| header.getDestination(i) == myAddr) {
			// TODO: cleanup destination list before pushing, i.e., include only destinations for this forwarder
			output(0).push(p); // Node is in the forwarder list
			return;
		}

	output(1).push(p); // Node is not in the forwarder list -> discard
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastForwarderClassifier)
