#include <click/config.h>
#include <click/args.hh>
#include "castor_add_xcast_pkt_tohistory.hh"
#include "castor_xcast.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorAddXcastPktToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("CastorHistory", ElementCastArg("CastorHistory"), history)
			.complete();
}

void CastorAddXcastPktToHistory::push(int, Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p);

	unsigned int i = 0;
	for(unsigned int j = 0; j < pkt.getNNextHops(); j++) {
		unsigned int pos = i;
		for(; i < pos + pkt.getNextHopNAssign(j); i++) {
			history->addPkt(pkt.getPid(i), pkt.getFlowId(), CastorAnno::src_id_anno(pkt.getPacket()), pkt.getNextHop(j), pkt.getDestination(i), p->timestamp_anno());
		}
	}

	output(0).push(pkt.getPacket());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddXcastPktToHistory)
