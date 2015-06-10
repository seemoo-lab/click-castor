#include <click/config.h>
#include <click/confparse.hh>
#include <click/args.hh>
#include "castor_add_ack_tohistory.hh"

CLICK_DECLS

int CastorAddAckToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			cpEnd);
}

void CastorAddAckToHistory::push(int, Packet *p) {
	CastorAck& ack = (CastorAck&) *p->data();
	const PacketId& pid = CastorPacket::getCastorAnno(p);
	if (history->hasAck(pid)) {
		history->addAckFor(pid, CastorPacket::src_ip_anno(p));
	} else {
		history->addFirstAckFor(pid, CastorPacket::src_ip_anno(p), ack.auth);
	}

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddAckToHistory)
