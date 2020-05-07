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

#ifndef CLICK_CASTOR_START_TIMER_HH
#define CLICK_CASTOR_START_TIMER_HH

#include <click/element.hh>
#include <click/timer.hh>
#include "../castor.hh"
#include "castor_routing_table.hh"
#include "castor_timeout_table.hh"
#include "castor_history.hh"
#include "../flow/castor_flow_table.hh"
#include "../ratelimiter/castor_rate_limit_table.hh"

CLICK_DECLS

class CastorStartTimer : public Element {
public:
	const char *class_name() const { return "CastorStartTimer"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	void run_timer(Timer*);

	class PidTimer : public Timer {
	public:
		PidTimer(CastorStartTimer *element, const PacketId pid, Timestamp timeout);
		inline const PacketId &getPid() const { return pid; }
	private:
		const PacketId pid;
	};

	void adjust_estimator(const PacketId&);
	void adjust_rate_limit(const PacketId& pid);

	CastorRoutingTable* table;
	CastorTimeoutTable* toTable;
	CastorHistory* history;
	CastorFlowTable* flowtable;
	CastorRateLimitTable* rate_limits;
	NeighborId myId;

	bool verbose;
};

CLICK_ENDDECLS

#endif
