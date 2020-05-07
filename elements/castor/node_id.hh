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

#ifndef CLICK_CASTOR_NODE_ID_HH
#define CLICK_CASTOR_NODE_ID_HH

#include <click/ipaddress.hh>

CLICK_DECLS

class NodeId : public IPAddress {
public:
    inline NodeId() : IPAddress() { }

    explicit inline NodeId(const unsigned char *data) : IPAddress(data) { }

    inline NodeId(const uninitialized_type &unused) : IPAddress(unused) { }

    inline NodeId(IPAddress addr) : IPAddress(addr.data()) { }

	static NodeId make_broadcast() {
		return static_cast<NodeId>(IPAddress::make_broadcast().data());
	}

	inline bool is_multicast() const {
		return IPAddress::is_multicast();
	}

    /** @brief Return a pointer to the address data. */
    inline unsigned char *data() {
    	return IPAddress::data();
    }

    /** @overload */
    inline const unsigned char *data() const {
    	return IPAddress::data();
    }

    inline bool empty() {
    	return IPAddress::empty();
    }
} CLICK_SIZE_PACKED_ATTRIBUTE;

class ArgContext;
class Args;
extern const ArgContext blank_args;

class NodeIdArg : public IPAddressArg {
};

template<> struct DefaultArg<NodeId> : public NodeIdArg {};
template<> struct has_trivial_copy<NodeId> : public true_type {};

CLICK_ENDDECLS

#endif
