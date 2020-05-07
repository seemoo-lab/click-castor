/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <click/config.h>
#include <click/args.hh>
#include <click/straccum.hh>
#include "castor_start_timer.hh"

CLICK_DECLS

CastorStartTimer::PidTimer::PidTimer(CastorStartTimer *element, const PacketId pid, Timestamp timeout)
: Timer(element), pid(pid) {
	initialize(element);
	schedule_after(timeout);
}

int CastorStartTimer::configure(Vector<String>& conf, ErrorHandler* errh) {
	table = NULL;
	rate_limits = NULL;
	return Args(conf, this, errh)
			.read_mp("TO", ElementCastArg("CastorTimeoutTable"), toTable)
			.read_mp("HISTORY", ElementCastArg("CastorHistory"), history)
			.read_p("RT", ElementCastArg("CastorRoutingTable"), table)
			.read_p("FLOW_TABLE", ElementCastArg("CastorFlowTable"), flowtable)
			.read_p("RATE_LIMITS", ElementCastArg("CastorRateLimitTable"), rate_limits)
			.read("ID", myId)
			.read_or_set("VERBOSE", verbose, false)
			.complete();
}

Packet* CastorStartTimer::simple_action(Packet* p) {
	// Add timer
	CastorPkt& header = (CastorPkt&) *p->data();
	Timestamp timeout = toTable->getTimeout(header.fid, history->routedTo(header.pid)).value();
	new PidTimer(this, header.pid, timeout);

	return p;
}

void CastorStartTimer::run_timer(Timer* _timer) {
	PidTimer *timer = (CastorStartTimer::PidTimer *)_timer;

	const PacketId& pid = timer->getPid();
	if (!history->hasAck(pid)) {
		if (table)       adjust_estimator(pid);
		if (rate_limits) adjust_rate_limit(pid);
	}
	/* replay protection, will not accept an ACK again in the future */
	flowtable->get(history->getFlowId(pid)).set_expired_pkt(history->k(pid));
	history->remove(pid);

	delete timer;
}

void CastorStartTimer::adjust_estimator(const PacketId& pid) {
	NeighborId routedTo = history->routedTo(pid);

	// Check whether PKT was broadcast, if yes, do nothing as we don't know who might have received it
	if (routedTo == NeighborId::make_broadcast())
		return;

	const FlowId& fid = history->getFlowId(pid);

	// decrease ratings
	if(verbose) {
		StringAccum sa;
		sa << "[" << Timestamp::now() << "@" << myId << "] Timeout: no ACK received from " << routedTo.unparse();
		click_chatter(sa.c_str());
	}
	CastorEstimator& estimator = table->at(fid)[routedTo];
	estimator.decreaseFrist();
	estimator.decreaseAll();
}

void CastorStartTimer::adjust_rate_limit(const PacketId& pid) {
	auto& senders = history->getPktSenders(pid);
	for (auto& sender : senders) {
		rate_limits->lookup(sender).decrease();
		rate_limits->notify(sender);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorStartTimer)
