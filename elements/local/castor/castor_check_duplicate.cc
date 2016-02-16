#include <click/config.h>
#include <click/args.hh>
#include "castor_check_duplicate.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorCheckDuplicate::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("CastorHistory", ElementCastArg("CastorHistory"), history)
			.complete();
}

void CastorCheckDuplicate::push(int, Packet *p) {
	CastorPkt& pkt = (CastorPkt&) *p->data();

	int port = 0; // default behavior: have never seen pid -> forward PKT (output port 0)

	if (history->hasPkt(pkt.pid)) {
		/**
		 * XXX: According to Castor technical paper: If a packet with same pid, but different eauth or payload is received, it should not be considered a duplicate.
		 * In that case, however, the timer should not be restarted.
		 */
		if (pkt.arq) {
			port = 4;
		} else if (history->hasPktFrom(pkt.pid, CastorAnno::src_id_anno(p))) {
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
