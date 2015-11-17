#ifndef CLICK_CASTOR_RATE_LIMITER_HH
#define CLICK_CASTOR_RATE_LIMITER_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/tokenbucket.hh>
#include <click/timer.hh>
#include "../node_id.hh"

CLICK_DECLS

class CastorRateLimiter : public Element {
public:
	const char *class_name() const { return "CastorRateLimiter"; }
	const char *port_count() const { return "1/1-2"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet* p);
private:
	class RingBuffer {
	public:
		RingBuffer(unsigned int capacity = default_capacity) : capacity(capacity), count(0), start(0) {
			store = new Packet*[capacity];
		};
		RingBuffer(const RingBuffer& x) : capacity(x.capacity), count(x.count), start(x.start) {
			store = new Packet*[capacity];
			memcpy(&store[0], &x.store[0], capacity * sizeof(Packet*));
		}
		~RingBuffer() {
			delete [] store;
		}
		unsigned int size() const {
			return count;
		}
		bool empty() const {
			return count == 0;
		}
		bool full() const {
			return count == capacity;
		}
		bool push(Packet* p) {
			assert(p != NULL);
			if (full()) return false;
			index_t end = add(start, count);
			store[end] = p;
			count++;
			return true;
		}
		Packet* pop() {
			if (empty()) return NULL;
			Packet* p = store[start];
			assert(p != NULL);
			count--;
			start = add(start, 1);
			return p;
		}
		friend class CastorRateLimiter;
	private:
		typedef unsigned int index_t;
		index_t start;
		index_t count;
		index_t capacity;
		static index_t default_capacity;

		Packet** store;

		inline index_t add(index_t a, index_t b) const { return (a + b) % capacity; }
	};

	class RateTimer : public Timer {
	public:
		void set_node(const NodeId& node) { _node = node; }
		const NodeId& node() const { return _node; }
	private:
		NodeId _node;
	};

	NodeId node_id;

	void run_timer(Timer*);
	void run_timer(RateTimer*);

	void emit_packet(const NodeId&);

	atomic_uint32_t drops;

	HashTable<const NodeId, RingBuffer> buckets;
	HashTable<const NodeId, TokenBucket> tokens;
	HashTable<const NodeId, RateTimer> timers;
};

CLICK_ENDDECLS

#endif
