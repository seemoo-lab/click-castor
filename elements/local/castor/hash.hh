#ifndef CLICK_CASTOR_HASH_HH
#define CLICK_CASTOR_HASH_HH

#define CASTOR_HASH_LENGTH 20

CLICK_DECLS

class Hash {
public:
	typedef unsigned long hashcode_t;

	inline Hash() { memset(&array, 0, sizeof(array)); }
	inline Hash(const uint8_t array[]) { memcpy(this->array, array, sizeof(this->array)); }
	inline hashcode_t hashcode() const {
		hashcode_t x;
		memcpy(&x, array, sizeof(hashcode_t));
		return x;
	}
	inline Hash& operator=(const Hash& x) {
		memcpy(&array, &x.array, sizeof(array));
		return *this;
	}
	inline const uint8_t& operator[](size_t i) const {
		assert(i < sizeof(array));
		return array[i];
	}
	inline uint8_t& operator[](size_t i) {
		assert(i < sizeof(array));
		return array[i];
	}
	inline const uint8_t* data() const {
		return array;
	}
	inline uint8_t* data() {
	    return array;
	}
	inline size_t size() const {
		return sizeof(array);
	}
	inline bool operator==(const Hash& x) const {
		return memcmp(this->array, x.array, sizeof(array)) == 0;
	}
	inline bool operator!=(const Hash& x) const {
		return memcmp(this->array, x.array, sizeof(array)) != 0;
	}
	inline String str() const {
		char buffer[2 * sizeof(array) + 1];
		for (size_t i = 0; i < sizeof(array); i++) {
			sprintf(buffer + 2 * i, "%02x", array[i]);
		}
		return String(buffer);
	}
private:
	uint8_t array[CASTOR_HASH_LENGTH];
};

CLICK_ENDDECLS

#endif
