#ifndef CLICK_CASTOR_NEIGHBOR_ID_HH
#define CLICK_CASTOR_NEIGHBOR_ID_HH

#include <click/etheraddress.hh>

CLICK_DECLS

class NeighborId : public EtherAddress {
public:
    inline NeighborId() : EtherAddress() { }

    explicit inline NeighborId(const unsigned char *data) : EtherAddress(data) { }

    inline NeighborId(const uninitialized_type &unused) : EtherAddress(unused) { }

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

private:
    inline NeighborId(EtherAddress ea) : EtherAddress(ea.data()) { }
};

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
