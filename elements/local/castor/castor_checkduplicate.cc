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
	CastorPkt& pkt = (CastorPkt&) *p->data();

	int port = 0; // default behavior: have never seen pid -> forward PKT (output port 0)

	if (history->hasPkt(pkt.pid)) {
		/**
		 * XXX: According to Castor technical paper: If a packet with same pid, but different eauth or payload is received, it should not be considered a duplicate.
		 * In that case, however, the timer should not be restarted.
		 */
		if (history->hasPktFrom(pkt.pid, CastorPacket::src_ip_anno(p))) {
			port = 3; // have received pid from same neighbor -> discard
		} else if (history->hasAck(pkt.pid)) {
			port = 1; // have received pid from different neighbor AND already know corresponding ACK -> retransmit ACK
		} else {
			port = 2; // have received pid from different neighbor AND do NOT already know corresponding ACK -> add PKT to history and discard
		}
	}

	output(port).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCheckDuplicate)
