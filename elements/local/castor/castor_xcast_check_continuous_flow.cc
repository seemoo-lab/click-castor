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

	const NextFlowAuth nfauth = crypto->hash(pkt.getFlowId());
	const FlowId* oldFid = fidTable->get(nfauth);
	if (oldFid) {
		const FlowId& newFid = pkt.getFlowId();
		for (int i = 0; i < pkt.getNDestinations(); i++) {
			const NodeId subflow = pkt.getDestination(i);
			if (!routingTable->copyFlowEntry(newFid, *oldFid, subflow))
				click_chatter("Warning: tried to override routing entry by continuous flow");
		}
		// FIXME wir müssen auch hier wieder <fid,subflow> separat betrachten.
		//       ein Gruppenknoten darf bspw. _nicht_ für einen anderen den nfauth bestätigen dürfen.
		fidTable->remove(nfauth); // we only try once, no longer needed
	}

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastCheckContinuousFlow)
