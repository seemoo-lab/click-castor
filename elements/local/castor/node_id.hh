#ifndef CLICK_CASTOR_NODE_ID_HH
#define CLICK_CASTOR_NODE_ID_HH

#include <click/etheraddress.hh>

CLICK_DECLS

class NodeId : public EtherAddress {
public:
    inline NodeId() : EtherAddress() { }

    explicit inline NodeId(const unsigned char *data) : EtherAddress(data) { }

    inline NodeId(const uninitialized_type &unused) : EtherAddress(unused) { }

	static NodeId make_broadcast() {
		return static_cast<NodeId>(EtherAddress::make_broadcast());
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

private:
    inline NodeId(EtherAddress ea) : EtherAddress(ea.data()) { }
};

class ArgContext;
class Args;
extern const ArgContext blank_args;

class NodeIdArg : public EtherAddressArg {
public:
    NodeIdArg(int flags = 0) : EtherAddressArg(flags) {}
};

template<> struct DefaultArg<NodeId> : public NodeIdArg {};
template<> struct has_trivial_copy<NodeId> : public true_type {};

CLICK_ENDDECLS

#endif
