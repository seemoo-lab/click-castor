#include <click/config.h>
#include <click/confparse.hh>
#include "castor_xcast_check_continuous_flow.hh"
#include "castor_xcast.hh"

CLICK_DECLS

int CastorXcastCheckContinuousFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh,
		"CastorNextFlowTable", cpkP+cpkM, cpElementCast, "CastorNextFlowTable", &fidTable,
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &routingTable,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
        cpEnd) < 0)
    	return -1;
    return 0;
}

void CastorXcastCheckContinuousFlow::push(int, Packet *p) {
	const CastorXcastPkt pkt(p);

	const FlowId& newFid = pkt.getFlowId();
	const NextFlowAuth nfauth = crypto->hash(newFid);
	for (int i = 0; i < pkt.getNDestinations(); i++) {
		const NodeId subflow = pkt.getDestination(i);
		const FlowId* oldFid = fidTable->get(nfauth, subflow);
		if (oldFid) {
			if (!routingTable->copyFlowEntry(newFid, *oldFid, subflow))
				click_chatter("[CastorXcastCheckContinuousFlow] Warning: tried to override routing entry by continuous flow (should not happen)");
			fidTable->remove(nfauth, subflow); // we only try once, no longer needed
		}
	}

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastCheckContinuousFlow)
