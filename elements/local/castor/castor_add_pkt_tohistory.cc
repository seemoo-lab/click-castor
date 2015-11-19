#include <click/config.h>
#include <click/confparse.hh>
#include "castor_add_pkt_tohistory.hh"
#include "castor.hh"

CLICK_DECLS

int CastorAddPktToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			cpEnd);
}

void CastorAddPktToHistory::push(int, Packet *p){
	CastorPkt& pkt = (CastorPkt&) *p->data();

	history->addPkt(pkt.pid, pkt.fid, CastorPacket::src_id_anno(p), CastorPacket::dst_id_anno(p), pkt.dst, p->timestamp_anno());

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddPktToHistory)
