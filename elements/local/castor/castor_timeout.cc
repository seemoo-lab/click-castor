#include <click/config.h>
#include <click/confparse.hh>
#include "castor_timeout.hh"

CLICK_DECLS

CastorTimeout::CastorTimeout() {
	timers = HashTable<Timer*,Entry>();
}

CastorTimeout::~CastorTimeout() {
}

int CastorTimeout::configure(Vector<String>& conf, ErrorHandler* errh) {
	// Default value from experimental setup of Castor technical paper
	timeout = 500;

	return cp_va_kparse(conf, this, errh,
			"CastorRoutingTable", cpkP + cpkM, cpElementCast, "CastorRoutingTable", &table,
			"CastorHistory", cpkP + cpkM, cpElementCast, "CastorHistory", &history,
			"TIMEOUT", cpkP, cpInteger, &timeout,
			cpEnd);
}

void CastorTimeout::push(int, Packet* p) {
	// Create timer
	Timer* timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(timeout);

	// Add timer
	Castor_PKT& header = (Castor_PKT&) *p->data();
	Entry entry;
	memcpy(entry.pid, header.pid, sizeof(PacketId));
	timers.set(timer, entry);

	output(0).push(p);
}

void CastorTimeout::run_timer(Timer* timer) {

	Entry* entry = timers.get_pointer(timer);
	if(!entry) {
		click_chatter("[%f] !!! Unknown timer fired", Timestamp::now().doubleval());
		return;
	}

	PacketId& pid = entry->pid;
	const IPAddress& routedTo = history->routedTo(pid);

	// Check whether ACK has been received in the meantime
	if (history->hasACK(pid)) {
		//click_chatter("[%f] Timeout: ACK received in the meantime from %s", Timestamp::now().doubleval(), routedTo.unparse().c_str());
		return;
	}

	history->setExpired(pid);

	// Check whether PKT was broadcast, if yes, do nothing
	// FIXME: Why is that?
	if (routedTo == IPAddress::make_broadcast()) {
		return;
	}

	// decrease ratings
	click_chatter("[%f] Timeout: no ACK received from %s", Timestamp::now().doubleval(), routedTo.unparse().c_str());
	table->updateEstimates(pid, routedTo, CastorRoutingTable::decrease, CastorRoutingTable::first);
	table->updateEstimates(pid, routedTo, CastorRoutingTable::decrease, CastorRoutingTable::all);

	// delete timer
	timers.erase(timer);
	delete timer;

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorTimeout)
