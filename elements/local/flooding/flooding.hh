#ifndef CLICK_FLOODING_HH
#define CLICK_FLOODING_HH

#include <click/packet_anno.hh>

CLICK_DECLS

class Flooding {

public:
	typedef unsigned long long Id;

	static inline Id getId(const Packet* p) {
		return *((Id*) getPayload(p));
	}

	static inline void setId(WritablePacket* p, Id id) {
		*((Id*) getPayload(p)) = id;
	}

private:
	static inline const uint8_t* getPayload(const Packet* p) {
		return (p->data() + sizeof(click_ip));
	}
	static inline uint8_t* getPayload(WritablePacket* p) {
		return (p->data() + sizeof(click_ip));
	}
};

CLICK_ENDDECLS
#endif /* CLICK_FLOODING_HH */
