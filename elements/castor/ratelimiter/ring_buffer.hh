#ifndef CLICK_RING_BUFFER_HH
#define CLICK_RING_BUFFER_HH

CLICK_DECLS

class RingBuffer {
public:
	RingBuffer(unsigned int capacity = 0) : _capacity(capacity), count(0), start(0) {
		store = new Packet*[capacity];
	};
	RingBuffer(const RingBuffer& x) : _capacity(x._capacity), count(x.count), start(x.start) {
		store = new Packet*[_capacity];
		memcpy(store, x.store, _capacity * sizeof(Packet*));
	}
	~RingBuffer() {
		delete [] store;
	}
	RingBuffer& operator=(const RingBuffer& x) {
		if (this == &x)
			return *this;
		if (_capacity != x._capacity) {
			delete [] store;
			_capacity = x._capacity;
			store = new Packet*[_capacity];
		}
		memcpy(store, x.store, _capacity);
		start = x.start;
		count = x.count;
		return *this;
	}
	unsigned int size() const {
		return count;
	}
	unsigned int capacity() const {
		return _capacity;
	}
	bool empty() const {
		return count == 0;
	}
	bool full() const {
		return count == _capacity;
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
	index_t _capacity;

	Packet** store;

	inline index_t add(index_t a, index_t b) const { return (a + b) % _capacity; }
};

CLICK_ENDDECLS

#endif
