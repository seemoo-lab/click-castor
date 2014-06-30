#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_dstclassifier.hh"

CLICK_DECLS

CastorDstClassifier::CastorDstClassifier() {
}

CastorDstClassifier::~CastorDstClassifier() {
}

int CastorDstClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh)
			.read_mp("DST", myAddr)
			.complete() < 0)
		return -1;
	return 0;
}

void CastorDstClassifier::push(int, Packet *p) {

	Castor_PKT* header = (Castor_PKT*) p->data();

	if (myAddr == header->dst) {
		output(0).push(p);
	} else {
		output(1).push(p);
	}

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDstClassifier)
