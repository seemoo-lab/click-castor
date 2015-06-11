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
	const CastorPkt& pkt = (CastorPkt&) *p->data();

	const NextFlowAuth nfauth = crypto->hash(pkt.fid);
	const FlowId* oldFid = fidTable->get(nfauth);
	if (oldFid) {
		const FlowId& newFid = pkt.fid;
		const NodeId& subflow = pkt.dst;
		if (!routingTable->copyFlowEntry(newFid, *oldFid, subflow))
			click_chatter("Warning: tried to override routing entry by continuous flow");
		fidTable->remove(nfauth); // we only try once, no longer needed
	}

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCheckContinuousFlow)
