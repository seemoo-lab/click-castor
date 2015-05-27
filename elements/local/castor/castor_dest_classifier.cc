#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_dest_classifier.hh"

CLICK_DECLS

int CastorDestClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh)
			.read_mp("NodeId", myId)
			.complete() < 0)
		return -1;
	return 0;
}

void CastorDestClassifier::push(int, Packet *p) {

	CastorPkt* header = (CastorPkt*) p->data();

	if (myId == header->dst) {
		output(0).push(p);
	} else {
		output(1).push(p);
	}

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDestClassifier)
