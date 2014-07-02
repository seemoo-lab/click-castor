#include <click/config.h>
#include <click/confparse.hh>
#include "castor_checkduplicate.hh"

CLICK_DECLS

CastorCheckDuplicate::CastorCheckDuplicate() {
}

CastorCheckDuplicate::~CastorCheckDuplicate() {
}

int CastorCheckDuplicate::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        cpEnd);
}

void CastorCheckDuplicate::push(int, Packet *p) {
	Castor_PKT& pkt = (Castor_PKT&) *p->data();

	if(history->hasPkt(pkt.pid)){
		/**
		 * TODO: If a packet with same pid, but different eauth or payload is received, it should not be considered a duplicate. In that case, however, the timer should not be restarted.
		 */
		output(1).push(p); // -> discard
	} else{
		output(0).push(p);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCheckDuplicate)
