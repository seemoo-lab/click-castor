#ifndef CLICK_BITSET_HH
#define CLICK_BITSET_HH

#import <click/straccum.hh>

CLICK_DECLS

class BitSet {
private:
	typedef unsigned int rep_t;
#define WORD_BITS (8 * sizeof(rep_t))
public:
	inline BitSet(size_t size) : size(size) {
		array = new rep_t[size / WORD_BITS + 1] { 0 };
	}
	~BitSet() { delete [] array; }
	inline void set(size_t i) {
		array[slot(i)] |= mask(i);
	}
	inline void clear(size_t i) {
		array[slot(i)] &= ~mask(i);
	}
	inline bool test(size_t i) const {
		return array[slot(i)] & mask(i);
	}
	String unparse() const {
		StringAccum sa;
		for (unsigned int i = 0; i < size; i++)
			sa << (test(i) ? "1" : "0");
		return sa.take_string();
	}
private:
	rep_t* array;
	size_t size;
	inline size_t slot(size_t i) const {
		return i / WORD_BITS;
	}
	inline rep_t mask(size_t i) const {
		return 1 << (i % WORD_BITS);
	}
};

CLICK_ENDDECLS

#endif
