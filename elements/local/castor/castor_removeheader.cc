#include <click/config.h>
#include <click/confparse.hh>
#include "castor_removeheader.hh"

CLICK_DECLS

CastorRemoveHeader::CastorRemoveHeader() {
}

CastorRemoveHeader::~CastorRemoveHeader() {
}

void CastorRemoveHeader::push(int, Packet* p){
	p->pull(sizeof(Castor_PKT));

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRemoveHeader)
