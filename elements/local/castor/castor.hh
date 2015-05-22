#ifndef CLICK_CASTOR_H
#define CLICK_CASTOR_H

#include <click/ipaddress.hh>
#include <click/packet_anno.hh>
#include "node_id.hh"

//#define DEBUG_ACK_SRCDST  // uncomment to add source and destination fields to ACK packets
#define DEBUG_HOPCOUNT // include (unprotected) hopcount field in packets

#define CASTOR_HASHLENGTH                           20
#define CASTOR_FLOWAUTH_ELEM                         8  // log2(CASTOR_FLOWSIZE)
#define CASTOR_FLOWSIZE		  (1<<CASTOR_FLOWAUTH_ELEM) // Number of flow auth elements in the header

CLICK_DECLS

struct CastorType { // C++11's strongly typed 'enum class' does not work, so create artificial namespace
	enum {
		PKT = 0xC0,
		ACK = 0xA0,

		DUMMY = 0x00,
		DUMMY_PKT = PKT | DUMMY,
		DUMMY_ACK = ACK | DUMMY,

		MERKLE = 0x0A,
		MERKLE_PKT = PKT | MERKLE,
		MERKLE_ACK = ACK | MERKLE,

		XCAST = 0x0C,
		XCAST_PKT = PKT | XCAST,
		XCAST_ACK = ACK | XCAST
	};
};

class Hash {
public:
	typedef unsigned long hashcode_t;

	inline Hash() { memset(&array, 0, sizeof(array)); }
	inline Hash(const uint8_t array[]) { memcpy(this->array, array, sizeof(this->array)); }
	inline hashcode_t hashcode() const {
		hashcode_t x;
		memcpy(&x, array, sizeof(hashcode_t));
		return x;
	}
	inline Hash& operator=(const Hash& x) {
		memcpy(&array, &x.array, sizeof(array));
		return *this;
	}
	inline const uint8_t& operator[](size_t i) const {
		assert(i < sizeof(array));
		return array[i];
	}
	inline uint8_t& operator[](size_t i) {
		assert(i < sizeof(array));
		return array[i];
	}
	inline const uint8_t* data() const {
		return array;
	}
	inline uint8_t* data() {
	    return array;
	}
	inline bool operator==(const Hash& x) const {
		return memcmp(this->array, x.array, sizeof(array)) == 0;
	}
	inline String str() const {
		char buffer[2 * sizeof(array) + 1];
		for (int i = 0; i < sizeof(array); i++) {
			sprintf(buffer + 2 * i, "%02x", array[i]);
		}
		return String(buffer);
	}
private:
	uint8_t array[CASTOR_HASHLENGTH];
};

typedef Hash FlowId;
typedef Hash PacketId;
typedef struct {
	Hash data;
} FlowAuthElement;
typedef FlowAuthElement FlowAuth[CASTOR_FLOWAUTH_ELEM];
typedef Hash ACKAuth;
typedef Hash EACKAuth;

// The Packet Header Structure
typedef struct {
	uint8_t 	type;
	uint8_t 	hsize;
	uint8_t 	fsize;
	uint8_t 	ctype;
	uint16_t 	len;
	uint16_t	packet_num; // the k-th packet of the current flow, necessary for flow validation (determines whether fauth[i] is left or right sibling in the Merkle tree)
	NodeId		src;
	NodeId		dst;
	FlowId	 	fid;
	PacketId 	pid;
	FlowAuth 	fauth;
	EACKAuth 	eauth;
#ifdef DEBUG_HOPCOUNT
	uint8_t		hopcount;
#endif
} Castor_PKT;

// The ACK Header Structure
typedef struct {
	uint8_t 	type;
	uint8_t 	hsize;
	uint16_t 	len;
#ifdef DEBUG_ACK_SRCDST
	NodeId		src;
	NodeId		dst;
#endif
	ACKAuth 	auth;
} Castor_ACK;

/**
 * The Castor Class with utility functions to handle Packet Processing
 */
class CastorPacket {
public:

	static inline uint8_t getType(const Packet* p) {
		uint8_t type = p->data()[0] & 0xF0;
		return type;
	}

	static inline IPAddress src_ip_anno(const Packet* p) {
		return IPAddress(p->anno_u32(src_ip_anno_offset));
	}

	static inline void set_src_ip_anno(Packet* p, IPAddress addr) {
		p->set_anno_u32(src_ip_anno_offset, addr.addr());
	}

	static inline IPAddress mac_ip_anno(const Packet* p) {
		return IPAddress(p->anno_u32(mac_ip_anno_offset));
	}

	static inline void set_mac_ip_anno(Packet* p, IPAddress addr) {
		p->set_anno_u32(mac_ip_anno_offset, addr.addr());
	}
	/**
	 * User annotation space for Castor
	 */
	static inline uint8_t* getCastorAnno(Packet* p) {
		uint8_t* cAnno = p->anno_u8();
		cAnno += castor_anno_offset;
		return cAnno;
	}

	static inline bool isXcast(Packet* p) {
		uint8_t type = p->data()[0] & 0x0F;
		return (type == CastorType::XCAST);
	}

private:
	static const uint8_t src_ip_anno_offset = DST_IP_ANNO_OFFSET + DST_IP_ANNO_SIZE; // = 4
	static const uint8_t mac_ip_anno_offset = src_ip_anno_offset + DST_IP_ANNO_SIZE; // = 8
	static const uint8_t castor_anno_offset = mac_ip_anno_offset + DST_IP_ANNO_SIZE; // = 12

};

CLICK_ENDDECLS
#endif
