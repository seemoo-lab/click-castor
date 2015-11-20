#include <click/config.h>
#include <click/args.hh>
#include "castor_annotate_pid.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorAnnotatePid::configure(Vector<String>& conf, ErrorHandler* errh) {
	return Args(conf, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorAnnotatePid::simple_action(Packet* p) {
	assert(CastorPacket::getType(p) == CastorType::ACK);
	CastorAck& ack = (CastorAck&) *p->data();

	PacketId pid = crypto->hash(ack.auth);
	CastorAnno::hash_anno(p) = pid;

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAnnotatePid)
