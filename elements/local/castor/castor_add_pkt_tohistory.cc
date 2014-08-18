#include <click/config.h>
#include <click/confparse.hh>
#include "castor_add_pkt_tohistory.hh"

CLICK_DECLS

CastorAddPKTToHistory::CastorAddPKTToHistory() {
}

CastorAddPKTToHistory::~CastorAddPKTToHistory() {
}

int CastorAddPKTToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			cpEnd);
}

void CastorAddPKTToHistory::push(int, Packet *p){
	Castor_PKT& pkt = (Castor_PKT&) *p->data();

	history->addPkt(pkt.pid, pkt.fid, CastorPacket::src_ip_anno(p), p->dst_ip_anno(), pkt.dst);

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddPKTToHistory)
