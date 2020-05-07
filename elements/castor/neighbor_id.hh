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

#ifndef CLICK_CASTOR_NEIGHBOR_ID_HH
#define CLICK_CASTOR_NEIGHBOR_ID_HH

#include <click/etheraddress.hh>

CLICK_DECLS

class NeighborId : public EtherAddress {
public:
    inline NeighborId() : EtherAddress() { }

    explicit inline NeighborId(const unsigned char *data) : EtherAddress(data) { }

    inline NeighborId(const uninitialized_type &unused) : EtherAddress(unused) { }

	inline NeighborId(EtherAddress ea) : EtherAddress(ea.data()) { }

	static NeighborId make_broadcast() {
		return static_cast<NeighborId>(EtherAddress::make_broadcast());
	}

    /** @brief Return a pointer to the address data. */
    inline unsigned char *data() {
    	return EtherAddress::data();
    }

    /** @overload */
    inline const unsigned char *data() const {
    	return EtherAddress::data();
    }

    inline bool empty() {
    	return (uint32_t) sdata()[0] + sdata()[1] + sdata()[2] == (uint32_t) 0;
    }
} CLICK_SIZE_PACKED_ATTRIBUTE;

class ArgContext;
class Args;
extern const ArgContext blank_args;

class NeighborIdArg : public EtherAddressArg {
public:
    NeighborIdArg(int flags = 0) : EtherAddressArg(flags) {}
};

template<> struct DefaultArg<NeighborId> : public NeighborIdArg {};
template<> struct has_trivial_copy<NeighborId> : public true_type {};

CLICK_ENDDECLS

#endif
