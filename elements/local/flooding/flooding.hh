#ifndef CLICK_FLOODING_HH
#define CLICK_FLOODING_HH

#include <click/packet_anno.hh>

CLICK_DECLS

class Flooding {

public:
	typedef unsigned long long Id;
	typedef unsigned int hopcount_t;

	static inline Id getId(const Packet* p) {
		return *getField<Id>(p, 0);
	}
	static inline void setId(WritablePacket* p, Id id) {
		*getField<Id>(p, 0) = id;
	}
	static inline hopcount_t getHopcount(const Packet* p) {
		return *getField<hopcount_t>(p, sizeof(Id));
	}
	static inline void setHopcount(WritablePacket* p, hopcount_t hc) {
		*getField<hopcount_t>(p, sizeof(Id)) = hc;
	}

private:
	template<typename T>
	static inline const T* getField(const Packet* p, unsigned int off) {
		return (T*) (p->data() + sizeof(click_ip) + off);
	}
	template<typename T>
	static inline T* getField(WritablePacket* p, unsigned int off) {
		return (T*) (p->data() + sizeof(click_ip) + off);
	}
};

CLICK_ENDDECLS
#endif /* CLICK_FLOODING_HH */
