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
	index_t _capacity;
	index_t count;
	index_t start;

	Packet** store;

	inline index_t add(index_t a, index_t b) const { return (a + b) % _capacity; }
};

CLICK_ENDDECLS

#endif
