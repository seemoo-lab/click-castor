#include <click/config.h>
#include <click/confparse.hh>
#include "castor_timeout.hh"

CLICK_DECLS

CastorTimeout::CastorTimeout() {
	timers = HashTable<Timer*,TimeoutEntry>();
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

	// Create ACK Timer
	Timer* timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(timeout);

	//click_chatter("[%.2f] Set timer to expire at %.2f", Timestamp::now().doubleval(), timer->expiry().doubleval());

	// Create entry
	Castor_PKT* header;
	header = (Castor_PKT*) p->data();
	TimeoutEntry entry;
	memcpy(&entry.fid, header->fid, sizeof(FlowId));
	memcpy(&entry.pid, header->pid, sizeof(PacketId));
	entry.routedTo = p->dst_ip_anno();

	timers.set(timer, entry);

	output(0).push(p);

}

void CastorTimeout::run_timer(Timer* timer) {

	TimeoutEntry* entry = timers.get_pointer(timer);
	if(!entry) {
		click_chatter("!!! [%f] Unknown timer fired", Timestamp::now().doubleval());
		return;
	}

	// Check whether ACK has been received in the meantime
	if (history->hasACK(entry->pid)) {
		//click_chatter("[%.2f] Timer fired, ACK received", Timestamp::now().doubleval());
		return;
	}

	// decrease ratings
	//click_chatter("[%.2f] Timer fired, ACK not received", Timestamp::now().doubleval());
	table->updateEstimates(entry->fid, entry->routedTo, CastorRoutingTable::decrease, CastorRoutingTable::first);
	table->updateEstimates(entry->fid, entry->routedTo, CastorRoutingTable::decrease, CastorRoutingTable::all);

	// delete timer
	timers.erase(timer);
	delete timer;

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorTimeout)
