#ifndef CLICK_CASTOR_RATE_LIMITER_HH
#define CLICK_CASTOR_RATE_LIMITER_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/tokenbucket.hh>
#include <click/timer.hh>
#include "../../neighbordiscovery/neighbor_id.hh"
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

	/** Make sure that a timer for node exists and is initialized */
	void verify_timer_is_init(const NeighborId&);
	/** Make sure a TokenBucket for node exists and is initialized */
	void verify_token_is_init(const NeighborId&);

	atomic_uint32_t drops;

	HashTable<const NeighborId, RingBuffer> buckets;
	HashTable<const NeighborId, TokenBucket> tokens;
	HashTable<const NeighborId, RateTimer> timers;

	CastorRateLimitTable* rate_limits;
	size_t capacity;
};

CLICK_ENDDECLS

#endif
