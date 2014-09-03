/*
 * flooding.hh
 *
 *  Created on: Sep 3, 2014
 *      Author: milan
 */

#ifndef CLICK_FLOODING_HH
#define CLICK_FLOODING_HH

#include <click/packet_anno.hh>

CLICK_DECLS

class Flooding {

public:
	typedef unsigned long Id;

	static inline Id getId(Packet* p) {
		const uint8_t* pt = p->data();
		pt += sizeof(click_ip);
		Id id;
		memcpy(&id, pt, sizeof(Id));
		return id;
	}

	static inline void setId(WritablePacket* p, Id id) {
		uint8_t* pt = p->data();
		pt += sizeof(click_ip);
		memcpy(pt, &id, sizeof(Id));
	}

	static inline uint8_t* getHashAnno(Packet* p) {
		uint8_t* anno = p->anno_u8();
		anno += DST_IP_ANNO_OFFSET + DST_IP_ANNO_SIZE;
		return anno;
	}

};

CLICK_ENDDECLS
#endif /* CLICK_FLOODING_HH */
