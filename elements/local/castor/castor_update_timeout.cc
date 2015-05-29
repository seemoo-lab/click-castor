#include <click/config.h>
#include <click/confparse.hh>
#include "castor_update_timeout.hh"

CLICK_DECLS

int CastorUpdateTimeout::configure(Vector<String>& conf, ErrorHandler* errh) {
	int result = cp_va_kparse(conf, this, errh,
			"CastorTimeoutTable", cpkP + cpkM, cpElementCast, "CastorTimeoutTable", &table,
			"CastorHistory", cpkP + cpkM, cpElementCast, "CastorHistory", &history,
			cpEnd);
	return result;
}

void CastorUpdateTimeout::push(int, Packet* p) {
	const PacketId& pid = (PacketId&) *CastorPacket::getCastorAnno(p);

	// Calculate new round-trip time sample
	const Timestamp& time_sent = history->getTimestamp(pid);
	const Timestamp& time_recv = p->timestamp_anno();
	unsigned int new_rtt = (time_recv.sec() - time_sent.sec()) * 1000 + (time_recv.msec() - time_sent.msec());

	// Get flow's timeout object
	const FlowId& fid = history->getFlowId(pid);
	NodeId subfid = history->getDestination(pid);
	NodeId routedTo = history->routedTo(pid);
	CastorTimeout& timeout = table->getTimeout(fid, subfid, routedTo);

	// Update timeout
	timeout.update(new_rtt);

    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateTimeout)
