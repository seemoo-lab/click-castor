#include <click/config.h>
#include "castor_xcast_remove_header.hh"
#include "castor_xcast.hh"

CLICK_DECLS

Packet* CastorXcastRemoveHeader::simple_action(Packet* p) {
	auto header_length = CastorXcastPkt(p).header_length();
	p->pull(header_length);
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastRemoveHeader)
