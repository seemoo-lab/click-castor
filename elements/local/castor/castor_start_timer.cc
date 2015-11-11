#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_start_timer.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorStartTimer::PidTimer::PidTimer(CastorStartTimer *element, const PacketId pid, unsigned int timeout)
: Timer(element), pid(pid) {
	initialize(element);
	schedule_after_msec(timeout);
}

int CastorStartTimer::configure(Vector<String>& conf, ErrorHandler* errh) {
	int result = cp_va_kparse(conf, this, errh,
			"CastorRoutingTable", cpkP + cpkM, cpElementCast, "CastorRoutingTable", &table,
			"CastorTimeoutTable", cpkP + cpkM, cpElementCast, "CastorTimeoutTable", &toTable,
			"CastorHistory", cpkP + cpkM, cpElementCast, "CastorHistory", &history,
			"VERBOSE", cpkN, cpBool, &verbose,
			"NodeId", cpkN, cpIPAddress, &myId,
			cpEnd);
	if(verbose && myId.empty()) {
		click_chatter("Need to provide node's id for verbosity");
		return -1;
	}
	return result;
}

void CastorStartTimer::push(int, Packet* p) {
	// Add timer
	if(CastorPacket::isXcast(p)) {
		CastorXcastPkt header = CastorXcastPkt(p);
		// Set timer for each destination individually
		for(unsigned int i = 0; i < header.getNDestinations(); i++) {
			const PacketId pid = header.getPid(i);
			unsigned int timeout = toTable->getTimeout(header.getFlowId(), header.getDestination(i), history->routedTo(pid)).value();
			new PidTimer(this, pid, timeout);
		}
	} else {
		CastorPkt& header = (CastorPkt&) *p->data();
		unsigned int timeout = toTable->getTimeout(header.fid, header.dst, history->routedTo(header.pid)).value();
		new PidTimer(this, header.pid, timeout);
	}

	output(0).push(p);
}

void CastorStartTimer::run_timer(Timer* _timer) {
	PidTimer *timer = (CastorStartTimer::PidTimer *)_timer;

	const PacketId& pid = timer->getPid();
	if (!history->hasAck(pid))
		adjust_rating(pid);
	history->remove(pid);

	delete timer;
}

void CastorStartTimer::adjust_rating(const PacketId& pid) {
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
	CastorEstimator& estimator = table->getEstimator(fid, destination, routedTo);
	estimator.decreaseFrist();
	estimator.decreaseAll();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorStartTimer)
