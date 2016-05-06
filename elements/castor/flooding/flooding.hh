#ifndef CLICK_FLOODING_HH
#define CLICK_FLOODING_HH

#include <click/packet_anno.hh>

CLICK_DECLS

class Flooding {

public:
	typedef unsigned long long Id;
	typedef unsigned int hopcount_t;

	static inline const Id& id(Packet* p) {
		return get<Id>(p, 0);
	}
	static inline Id& id(WritablePacket* p) {
		return get<Id>(p, 0);
	}
	static inline const hopcount_t& hopcount(Packet* p) {
		return get<hopcount_t>(p, sizeof(Id));
	}
	static inline hopcount_t& hopcount(WritablePacket* p) {
		return get<hopcount_t>(p, sizeof(Id));
	}

private:
	template<typename T>
	static inline const T& get(Packet* p, unsigned int off) {
		return reinterpret_cast<const T&>(*(p->data() + sizeof(click_ip) + off));
	}
	template<typename T>
	static inline T& get(WritablePacket* p, unsigned int off) {
		return reinterpret_cast<T&>(*(p->data() + sizeof(click_ip) + off));
	}
};

CLICK_ENDDECLS
#endif /* CLICK_FLOODING_HH */
