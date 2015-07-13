#include <click/config.h>
#include <click/confparse.hh>
#include "castor_check_continuous_flow.hh"

CLICK_DECLS

int CastorCheckContinuousFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh,
		"CastorNextFlowTable", cpkP+cpkM, cpElementCast, "CastorNextFlowTable", &fidTable,
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &routingTable,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
        cpEnd) < 0)
    	return -1;
    return 0;
}

void CastorCheckContinuousFlow::push(int, Packet *p) {
#ifdef CASTOR_CONTINUOUS_FLOW
	const CastorPkt& pkt = (CastorPkt&) *p->data();

	const FlowId& newFid = pkt.fid;
	const NextFlowAuth nfauth = crypto->hash(newFid);
	const FlowId* oldFid = fidTable->get(nfauth, pkt.dst);
	if (oldFid) {
		const NodeId& subflow = pkt.dst;
		if (!routingTable->copyFlowEntry(newFid, *oldFid, subflow))
			click_chatter("[CastorCheckContinuousFlow] Warning: tried to override routing entry by continuous flow (should not happen)");
		fidTable->remove(nfauth, pkt.dst); // we only try once, no longer needed
	}
#endif

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCheckContinuousFlow)
