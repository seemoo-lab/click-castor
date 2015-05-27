#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_timeout.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorTimeout::PidTimer::PidTimer(CastorTimeout *element, const PacketId pid)
: Timer(element), pid(pid) {
	initialize(element);
	schedule_after_msec(element->getTimeout());
}

int CastorTimeout::configure(Vector<String>& conf, ErrorHandler* errh) {
	int result = cp_va_kparse(conf, this, errh,
			"CastorRoutingTable", cpkP + cpkM, cpElementCast, "CastorRoutingTable", &table,
			"CastorHistory", cpkP + cpkM, cpElementCast, "CastorHistory", &history,
			"TIMEOUT", cpkP + cpkM, cpInteger, &timeout,
			"NodeId", cpkN, cpIPAddress, &myId,
			"VERBOSE", cpkN, cpBool, &verbose,
			cpEnd);
	if(verbose && myId.empty()) {
		click_chatter("Need to provide node's id for verbosity");
		return -1;
	}
	return result;
}

void CastorTimeout::push(int, Packet* p) {
	// Add timer
	if(CastorPacket::isXcast(p)) {
		CastorXcastPkt header = CastorXcastPkt(p);
		// Set timer for each destination individually
		for(unsigned int i = 0; i < header.getNDestinations(); i++) {
			new PidTimer(this, header.getPid(i));
		}
	} else {
		CastorPkt& header = (CastorPkt&) *p->data();
		new PidTimer(this, header.pid);
	}

	output(0).push(p);
}

void CastorTimeout::run_timer(Timer* _timer) {
	PidTimer *timer = (CastorTimeout::PidTimer *)_timer;

	const PacketId& pid = timer->getPid();

	// delete timer, done with it
	delete timer;

	// Check whether ACK has been received in the meantime
	if (history->hasAck(pid))
		return;

	history->setExpired(pid);
	NodeId routedTo = history->routedTo(pid);

	// Check whether PKT was broadcast, if yes, do nothing as we don't know who might have received it
	if (routedTo == NodeId::make_broadcast())
		return;

	const FlowId& fid = history->getFlowId(pid);
	NodeId destination = history->getDestination(pid);

	// decrease ratings
	if(verbose) {
		StringAccum sa;
		sa << "[" << Timestamp::now() << "@" << myId << "] Timeout: no ACK received from " << routedTo.unparse();
		click_chatter(sa.c_str());
	}
	table->updateEstimates(fid, destination, routedTo, CastorRoutingTable::decrease, CastorRoutingTable::first);
	table->updateEstimates(fid, destination, routedTo, CastorRoutingTable::decrease, CastorRoutingTable::all);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorTimeout)
