#include <click/config.h>
#include <click/confparse.hh>
#include "castor_add_xcast_pkt_tohistory.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorAddXcastPktToHistory::CastorAddXcastPktToHistory() {
}

CastorAddXcastPktToHistory::~CastorAddXcastPktToHistory() {
}

int CastorAddXcastPktToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			cpEnd);
}

void CastorAddXcastPktToHistory::push(int, Packet *p){
	CastorXcastPkt pkt = CastorXcastPkt(p);

	unsigned int i = 0;
	for(unsigned int j = 0; j < pkt.getNNextHops(); j++) {
		unsigned int pos = i;
		for(; i < pos + pkt.getNextHopNAssign(j); i++) {
			history->addPkt(pkt.getPid(i), pkt.getFlowId(), pkt.getNextHop(j), pkt.getDestination(i));
		}
	}

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddXcastPktToHistory)
