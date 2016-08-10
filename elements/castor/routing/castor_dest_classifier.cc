#include <click/config.h>
#include <click/args.hh>
#include "castor_dest_classifier.hh"

CLICK_DECLS

int CastorDestClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("NodeId", myId)
			.complete();
}

void CastorDestClassifier::push(int, Packet *p) {
	CastorPkt* header = (CastorPkt*) p->data();

	click_chatter("CastorDestClassifier: %lx=%lx (myId=dst)\n", myId, header->dst);

	if (myId == header->dst)
		output(0).push(p);
	else
		output(1).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDestClassifier)
