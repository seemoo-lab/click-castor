#include <click/config.h>
#include <click/confparse.hh>
#include "castor_remove_header.hh"

CLICK_DECLS

void CastorRemoveHeader::push(int, Packet* p){
	p->pull(sizeof(CastorPkt));

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRemoveHeader)
