#include <click/config.h>
#include <click/args.hh>
#include "castor_no_loopback.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorNoLoopback::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("CastorHistory", ElementCastArg("CastorHistory"), history)
			.read_mp("ID", id)
			.complete();
}

void CastorNoLoopback::push(int, Packet* p) {
	const PacketId& pid = CastorAnno::hash_anno(p);
	if(history->hasPktFrom(pid, id))
		output(1).push(p);  // ACK arrived at source of corresponding PKT
	else
		output(0).push(p);  // ACK arrived at intermediate network node
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorNoLoopback)
