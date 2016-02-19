#include <click/config.h>
#include <click/args.hh>
#include "castor_add_ack_tohistory.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorAddAckToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("History", ElementCastArg("CastorHistory"), history)
			.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), flowtable)
			.complete();
}

Packet* CastorAddAckToHistory::simple_action(Packet *p) {
	CastorAck& ack = (CastorAck&) *p->data();
	const PacketId& pid = CastorAnno::hash_anno(p);
	if (history->hasAck(pid)) {
		history->addAckFor(pid, CastorAnno::src_id_anno(p));
	} else {
		history->addFirstAckFor(pid, CastorAnno::src_id_anno(p), ack.auth);
		flowtable->get(history->getFlowId(pid)).set_ack(history->k(pid));
	}
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddAckToHistory)
