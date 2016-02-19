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
};

class ArgContext;
class Args;
extern const ArgContext blank_args;

class NodeIdArg : public IPAddressArg {
};

template<> struct DefaultArg<NodeId> : public NodeIdArg {};
template<> struct has_trivial_copy<NodeId> : public true_type {};

CLICK_ENDDECLS

#endif
