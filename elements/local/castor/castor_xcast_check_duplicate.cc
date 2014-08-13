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
        "ADDR", cpkP+cpkM, cpIPAddress, &myAddr,
        cpEnd);
}

void CastorXcastCheckDuplicate::push(int, Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p);

	for(unsigned int i = 0; i < pkt.getNDestinations(); i++)
		if(history->hasPkt(pkt.getPid(i))) {
			pkt.removeDestination(pkt.getDestination(i)); // XXX not very efficient to delete one by one
			i--; // current position has changed, try again
		}

	bool isDuplicate = pkt.getNDestinations() == 0;

	if(isDuplicate) {
		/**
		 * XXX: According to Castor technical paper: If a packet with same pid, but different eauth or payload is received, it should not be considered a duplicate.
		 * In that case, however, the timer should not be restarted.
		 */
		output(1).push(pkt.getPacket()); // -> discard
	} else {
		pkt.setSingleNextHop(myAddr);
		output(0).push(pkt.getPacket());
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastCheckDuplicate)
