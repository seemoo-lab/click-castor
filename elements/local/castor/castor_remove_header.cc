#include <click/config.h>
#include "castor_remove_header.hh"
#include "castor.hh"

CLICK_DECLS

Packet* CastorRemoveHeader::simple_action(Packet* p){
	p->pull(sizeof(CastorPkt));
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRemoveHeader)
