#include <click/config.h>
#include <click/confparse.hh>
#include "castor_update_estimates.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorUpdateEstimates::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &table,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        cpEnd);
}

void CastorUpdateEstimates::push(int, Packet *p) {
	const PacketId& pid = CastorAnno::hash_anno(p);

	const auto& fid = history->getFlowId(pid);
	const auto& subfid = history->getDestination(pid);
	const auto& routedTo = history->routedTo(pid);
	const auto& from = CastorAnno::src_id_anno(p);
	bool isFirstAck = !history->hasAck(pid);

	CastorEstimator& estimator = table->getEstimator(fid, subfid, from);
	if (routedTo == from || isFirstAck)
		estimator.increaseFirst();
	estimator.increaseAll();

    output(isFirstAck ? 0 : 1).push(p); // only forward 1st ACK on default output port 0
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
