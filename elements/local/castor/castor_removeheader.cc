#include <click/config.h>
#include <click/confparse.hh>
#include "castor_removeheader.hh"

CLICK_DECLS

CastorRemoveHeader::CastorRemoveHeader() {
}

CastorRemoveHeader::~CastorRemoveHeader() {
}

void CastorRemoveHeader::push(int, Packet* p){
	WritablePacket *q = p->uniqueify();
	if(!q)
		return;

	q->pull(sizeof(Castor_PKT));

	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRemoveHeader)
