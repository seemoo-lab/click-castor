#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_dest_classifier.hh"

CLICK_DECLS

CastorDestClassifier::CastorDestClassifier() {
}

CastorDestClassifier::~CastorDestClassifier() {
}

int CastorDestClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh)
			.read_mp("ADDR", myAddr)
			.complete() < 0)
		return -1;
	return 0;
}

void CastorDestClassifier::push(int, Packet *p) {

	Castor_PKT* header = (Castor_PKT*) p->data();

	if (myAddr == header->dst) {
		output(0).push(p);
	} else {
		output(1).push(p);
	}

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDestClassifier)
