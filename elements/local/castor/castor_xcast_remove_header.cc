#include <click/config.h>
#include <click/confparse.hh>
#include "castor_xcast_remove_header.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastRemoveHeader::CastorXcastRemoveHeader() {
}

CastorXcastRemoveHeader::~CastorXcastRemoveHeader() {
}

void CastorXcastRemoveHeader::push(int, Packet* p) {
	CastorXcastPkt pkt(p);

	p->pull(pkt.getLength());

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastRemoveHeader)
