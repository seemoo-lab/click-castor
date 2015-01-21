#include <click/config.h>
#include <click/confparse.hh>
#include "castor_checkduplicate.hh"

CLICK_DECLS

int CastorCheckDuplicate::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        cpEnd);
}

void CastorCheckDuplicate::push(int, Packet *p) {
	Castor_PKT& pkt = (Castor_PKT&) *p->data();

	if(history->hasPkt(pkt.pid)) {
		/**
		 * XXX: According to Castor technical paper: If a packet with same pid, but different eauth or payload is received, it should not be considered a duplicate.
		 * In that case, however, the timer should not be restarted.
		 */
		if (history->hasPktFrom(pkt.pid, CastorPacket::src_ip_anno(p))) {
			output(3).push(p); // have received pid from same neighbor -> discard
		} else if (history->hasAck(pkt.pid)) {
			output(1).push(p); // have received pid from different neighbor AND already know corresponding ACK -> retransmit ACK
		} else {
			output(2).push(p); // have received pid from different neighbor AND do NOT already know corresponding ACK -> add PKT to history and discard
		}
	} else{
		output(0).push(p); // never seen pid -> forward
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCheckDuplicate)
