#include <click/config.h>
#include <click/confparse.hh>
#include "castor_xcast_check_duplicate.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastCheckDuplicate::CastorXcastCheckDuplicate() {
}

CastorXcastCheckDuplicate::~CastorXcastCheckDuplicate() {
}

int CastorXcastCheckDuplicate::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        cpEnd);
}

void CastorXcastCheckDuplicate::push(int, Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p);

	// FIXME Adapt for Xcast: if any pid is new, forward, but only for those pkts

	bool isDuplicate = true;
	for(unsigned int i = 0; i < pkt.getNDestinations(); i++)
		if(!history->hasPkt(pkt.getPid(i)))
			isDuplicate = false;

	if(isDuplicate){
		/**
		 * XXX: According to Castor technical paper: If a packet with same pid, but different eauth or payload is received, it should not be considered a duplicate. In that case, however, the timer should not be restarted.
		 */
		output(1).push(p); // -> discard
	} else{
		output(0).push(p);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastCheckDuplicate)
