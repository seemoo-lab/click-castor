/*
 * castor_timeout.cc
 *
 *  Created on: Jun 19, 2014
 *      Author: milan
 */

#include <click/config.h>
#include <click/confparse.hh>
#include "castor_timeout.hh"

CLICK_DECLS

CastorTimeout::CastorTimeout() {
	_timers = HashTable<Timer*,TimeoutEntry>();
}

CastorTimeout::~CastorTimeout() {
}

int CastorTimeout::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh, "CastorRoutingTable", cpkP + cpkM,
			cpElementCast, "CastorRoutingTable", &_table, "CastorHistory",
			cpkP + cpkM, cpElementCast, "CastorHistory", &_history,
			cpEnd);
}

void CastorTimeout::create_timer(Packet* p) {

	// Create ACK Timer
	Timer* timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(TIMEOUT_MS);

	//click_chatter("[%.2f] Set timer to expire at %.2f", Timestamp::now().doubleval(), timer->expiry().doubleval());

	// Create entry
	Castor_PKT* header;
	header = (Castor_PKT*) p->data();
	TimeoutEntry entry;
	memcpy(&entry.fid, header->fid, sizeof(FlowId));
	memcpy(&entry.pid, header->pid, sizeof(PacketId));
	entry.routedTo = p->dst_ip_anno();

	_timers.set(timer, entry);

}

void CastorTimeout::run_timer(Timer* timer) {

	TimeoutEntry* entry = _timers.get_pointer(timer);
	if(!entry) {
		click_chatter("!!! [%.2f] Unknown timer fired", Timestamp::now().doubleval());
		return;
	}

	// Check whether ACK has been received in the meantime
	if (_history->hasACK(entry->pid)) {
		//click_chatter("[%.2f] Timer fired, ACK received", Timestamp::now().doubleval());
		return;
	}

	// decrease ratings
	//click_chatter("[%.2f] Timer fired, ACK not received", Timestamp::now().doubleval());
	_table->updateEstimates(entry->fid, entry->routedTo, decrease, first);
	_table->updateEstimates(entry->fid, entry->routedTo, decrease, all);

	// delete timer
	_timers.erase(timer);
	delete timer;

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorTimeout)
