#include <click/config.h>
#include <click/confparse.hh>
#include "castor_update_estimates.hh"
#include "castor.hh"

CLICK_DECLS

int CastorUpdateEstimates::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &table,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        cpEnd);
}

void CastorUpdateEstimates::push(int, Packet *p) {
	const PacketId& pid = (PacketId&) *CastorPacket::getCastorAnno(p);

	const FlowId& fid = history->getFlowId(pid);
	NodeId subfid = history->getDestination(pid);
	NodeId routedTo = history->routedTo(pid);
	NodeId from = CastorPacket::src_ip_anno(p);
	bool isFirstAck = !history->hasAck(pid);

	CastorEstimator& estimator = table->getEstimator(fid, subfid, from);
	if (routedTo == from || isFirstAck)
		estimator.increaseFirst();
	estimator.increaseAll();

    output(isFirstAck ? 0 : 1).push(p); // only forward 1st ACK on default output port 0
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
