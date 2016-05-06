#include <click/config.h>
#include "castor_remove_header.hh"
#include "castor.hh"

CLICK_DECLS

Packet* CastorRemoveHeader::simple_action(Packet* p){
	CastorPkt& pkt = (CastorPkt&) *p->data();
	p->pull(pkt.header_len());
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRemoveHeader)
