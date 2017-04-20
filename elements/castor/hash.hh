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

#ifndef CLICK_CASTOR_HASH_HH
#define CLICK_CASTOR_HASH_HH

#define CASTOR_HASH_LENGTH 16

CLICK_DECLS

template<unsigned int S>
class Buffer {
public:
	typedef unsigned long hashcode_t;

	inline Buffer() { memset(&array, 0, S); }
	inline Buffer(const uint8_t array[]) { memcpy(this->array, array, S); }
	inline hashcode_t hashcode() const {
		hashcode_t x;
		memcpy(&x, array, S < sizeof(hashcode_t) ? S : sizeof(hashcode_t));
		return x;
	}
	inline Buffer<S>& operator=(const Buffer<S>& x) {
		memcpy(&array, &x.array, S);
		return *this;
	}
	inline const uint8_t& operator[](size_t i) const {
		assert(i < S);
		return array[i];
	}
	inline uint8_t& operator[](size_t i) {
		assert(i < S);
		return array[i];
	}
	inline const uint8_t* data() const {
		return array;
	}
	inline uint8_t* data() {
	    return array;
	}
	inline size_t size() const {
		return S;
	}
	inline bool operator==(const Buffer<S>& x) const {
		return memcmp(this->array, x.array, S) == 0;
	}
	inline bool operator!=(const Buffer<S>& x) const {
		return memcmp(this->array, x.array, S) != 0;
	}
	template<unsigned int S2>
	inline Buffer<(S+S2)> operator+(const Buffer<S2>& x) const {
		Buffer<S+S2> tmp;
		memcpy(tmp.data(), this->data(), S);
		memcpy(tmp.data() + S, x.data(), S2);
		return tmp;
	}
	inline String str() const {
		char buffer[2 * S + 1];
		for (size_t i = 0; i < S; i++) {
			sprintf(buffer + 2 * i, "%02x", array[i]);
		}
		return String(buffer);
	}
private:
	uint8_t array[S];
};

typedef Buffer<CASTOR_HASH_LENGTH> Hash;

CLICK_ENDDECLS

#endif
