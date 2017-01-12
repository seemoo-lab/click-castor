#include <click/config.h>
#include <click/args.hh>
#include "castor_has_complete_flow.hh"
#include "../castor.hh"

CLICK_DECLS

int CastorHasCompleteFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
		.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), flowtable)
		.complete();
}

Packet* CastorHasCompleteFlow::simple_action(Packet* p) {
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());

	if (!flowtable->has(pkt.fid) || !flowtable->get(pkt.fid).complete()) {
		checked_output_push(1, p);
		return 0;
	}
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorHasCompleteFlow)
