#include <click/config.h>
#include <click/confparse.hh>
#include <click/args.hh>
#include <click/error.hh>
#include "castor_add_ack_tohistory.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorAddAckToHistory::CastorAddAckToHistory() {
}

CastorAddAckToHistory::~CastorAddAckToHistory() {
}

int CastorAddAckToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	String dst_str;

	if(cp_va_kparse(conf, this, errh,
			"Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			cpEnd) < 0)
		return -1;

	return 0;
}

void CastorAddAckToHistory::push(int, Packet *p) {

	if(CastorPacket::isXcast(p)) {
		PacketId pid;
		CastorXcastAck& ack = (CastorXcastAck&) *p->data();
		crypto->hash(pid, ack.auth, ack.esize);
		if (history->hasAck(pid)) {
			history->addAckFor(pid, CastorPacket::src_ip_anno(p));
		} else {
			history->addFirstAckForXcastor(pid, CastorPacket::src_ip_anno(p), ack.auth);
		}
	} else {
		PacketId pid;
		Castor_ACK& ack = (Castor_ACK&) *p->data();
		crypto->hash(pid, ack.auth, ack.hsize);
		if (history->hasAck(pid)) {
			history->addAckFor(pid, CastorPacket::src_ip_anno(p));
		} else {
			history->addFirstAckForCastor(pid, CastorPacket::src_ip_anno(p), ack.auth);
		}
	}

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddAckToHistory)
