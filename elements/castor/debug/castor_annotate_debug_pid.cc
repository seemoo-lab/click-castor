#include <click/config.h>
#include <click/args.hh>
#include "../castor.hh"
#include "../castor_anno.hh"
#include "castor_annotate_debug_pid.hh"

CLICK_DECLS

Packet* CastorAnnotateDebugPid::simple_action(Packet* p) {
	assert(CastorPacket::getType(p) == CastorType::ACK);
	CastorAck& ack = (CastorAck&) *p->data();

	CastorAnno::hash_anno(p) = ack.auth;

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAnnotateDebugPid)
