#ifndef CLICK_RING_BUFFER_HH
#define CLICK_RING_BUFFER_HH

CLICK_DECLS

class RingBuffer {
public:
	RingBuffer(unsigned int capacity = 0) : capacity(capacity), count(0), start(0) {
		store = new Packet*[capacity];
	};
	RingBuffer(const RingBuffer& x) : capacity(x.capacity), count(x.count), start(x.start) {
		store = new Packet*[capacity];
		memcpy(store, x.store, capacity * sizeof(Packet*));
	}
	~RingBuffer() {
		delete [] store;
	}
	RingBuffer& operator=(const RingBuffer& x) {
		if (this == &x)
			return *this;
		if (capacity != x.capacity) {
			delete [] store;
			capacity = x.capacity;
			store = new Packet*[capacity];
		}
		memcpy(store, x.store, capacity);
		start = x.start;
		count = x.count;
		return *this;
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

	Packet** store;

	inline index_t add(index_t a, index_t b) const { return (a + b) % capacity; }
};

CLICK_ENDDECLS

#endif
