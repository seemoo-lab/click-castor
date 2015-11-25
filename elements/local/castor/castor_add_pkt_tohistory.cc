#include <click/config.h>
#include <click/args.hh>
#include "castor_add_pkt_tohistory.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorAddPktToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("CastorHistory", ElementCastArg("CastorHistory"), history)
			.complete();
}

void CastorAddPktToHistory::push(int, Packet *p){
	CastorPkt& pkt = (CastorPkt&) *p->data();

	history->addPkt(pkt.pid, pkt.fid, CastorAnno::src_id_anno(p), CastorAnno::dst_id_anno(p), pkt.dst, p->timestamp_anno());

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddPktToHistory)
