#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_xcast_dest_classifier.hh"

CLICK_DECLS

CastorXcastDestClassifier::CastorXcastDestClassifier() {
}

CastorXcastDestClassifier::~CastorXcastDestClassifier() {
}

int CastorXcastDestClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ADDR", myAddr)
			.complete();
}

void CastorXcastDestClassifier::push(int, Packet *p) {

	CastorXcastPkt header = CastorXcastPkt(p);

	bool delivered = false;
	bool forwarded = false;

	for (unsigned int i = 0; i < header.getNDestinations(); i++)
		if (!delivered && myAddr == header.getDestination(i)) {
			delivered = true;
			output(0).push(p); // local node is destination
		}

	if (header.getNDestinations() > (delivered ? 1 : 0)) {
		forwarded = true;
		Packet* q;
		if(delivered)
			q = p->clone()->uniqueify();
		else
			q = p;
		output(1).push(p);
	}

	assert(delivered || forwarded);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastDestClassifier)
