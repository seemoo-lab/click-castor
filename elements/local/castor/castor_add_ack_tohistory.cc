#include <click/config.h>
#include <click/confparse.hh>
#include <click/args.hh>
#include <click/error.hh>
#include "castor_add_ack_tohistory.hh"
#include "castor_xcast.hh"

CLICK_DECLS

int CastorAddAckToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			cpEnd);
}

void CastorAddAckToHistory::push(int, Packet *p) {

	if(CastorPacket::isXcast(p)) {
		CastorXcastAck& ack = (CastorXcastAck&) *p->data();
		SValue hash = crypto->hash(SValue(ack.auth.data(), ack.esize));
		PacketId pid(hash.begin());
		if (history->hasAck(pid)) {
			history->addAckFor(pid, CastorPacket::src_ip_anno(p));
		} else {
			history->addFirstAckForXcastor(pid, CastorPacket::src_ip_anno(p), ack.auth);
		}
	} else {
		Castor_ACK& ack = (Castor_ACK&) *p->data();
		SValue hash = crypto->hash(SValue(ack.auth.data(), ack.hsize));
		PacketId pid(hash.begin());
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
