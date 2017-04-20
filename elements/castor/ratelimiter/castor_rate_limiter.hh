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

#ifndef CLICK_CASTOR_RATE_LIMITER_HH
#define CLICK_CASTOR_RATE_LIMITER_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/tokenbucket.hh>
#include <click/timer.hh>
#include "../neighbor_id.hh"
#include "castor_rate_limit_table.hh"
#include "ring_buffer.hh"

CLICK_DECLS

class CastorRateLimiter : public Element {
public:
	const char *class_name() const { return "CastorRateLimiter"; }
	const char *port_count() const { return PORTS_1_1X2; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler*);

	void push(int, Packet* p);

	void update(const NeighborId&);
private:
	class RateTimer : public Timer {
	public:
		void set_node(const NeighborId& node) { _node = node; }
		const NeighborId& node() const { return _node; }
	private:
		NeighborId _node;
	};

	void run_timer(Timer*);
	void run_timer(RateTimer*);

	void emit_packet(const NeighborId&);

	void verify_entry_is_init(const NeighborId&);

	atomic_uint32_t drops;

	class RateLimitEntry {
	public:
		~RateLimitEntry() {
			timer.clear();
		}
		RingBuffer bucket;
		TokenBucket tokens;
		RateTimer timer;
	};
	HashTable<const NeighborId, RateLimitEntry> entries;

	bool enable;
	CastorRateLimitTable* rate_limits;
	size_t capacity;
};

CLICK_ENDDECLS

#endif
