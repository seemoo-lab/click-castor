#ifndef CLICK_CASTOR_HH
#define CLICK_CASTOR_HH

#include <click/ipaddress.hh>
#include <click/packet_anno.hh>
#include "node_id.hh"
#include "hash.hh"

//#define DEBUG_ACK_SRCDST  // uncomment to add source and destination fields to ACK packets
#define DEBUG_HOPCOUNT // include (unprotected) hopcount field in packets

#define CASTOR_CONTINUOUS_FLOW

#define CASTOR_FLOWAUTH_ELEM                         8  // Number of flow auth elements

CLICK_DECLS

namespace CastorType { // C++11's strongly typed 'enum class' does not work, so create artificial namespace
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
	};
};

typedef Hash FlowId;
typedef Hash PacketId;
typedef Hash FlowAuth[CASTOR_FLOWAUTH_ELEM];
typedef Hash AckAuth;
typedef Hash PktAuth;
#ifdef CASTOR_CONTINUOUS_FLOW
typedef Hash NextFlowAuth; // Authenticates the next flow ID
#endif

/**
 * The Castor data packet header (PKT)
 */
class CastorPkt {
public:
	uint8_t 	type;
	uint8_t 	hsize;
	uint16_t 	len;
	uint8_t 	fsize;
	uint8_t 	ctype;
	uint16_t	kpkt; // the k-th packet of the current flow, necessary for flow validation (determines whether fauth[i] is left or right sibling in the Merkle tree)
	NodeId		src;
	NodeId		dst;
	FlowId	 	fid;
#ifdef CASTOR_CONTINUOUS_FLOW
	NextFlowAuth nfauth;
#endif
	PacketId 	pid;
	FlowAuth 	fauth;
	PktAuth 	pauth;
#ifdef DEBUG_HOPCOUNT
	uint8_t		hopcount;
#endif
};

/**
 * The Castor acknowledgement packet (ACK)
 */
class CastorAck {
public:
	uint8_t 	type;
	uint8_t 	hsize;
	uint16_t 	len;
#ifdef DEBUG_ACK_SRCDST
	NodeId		src;
	NodeId		dst;
#endif
	AckAuth 	auth;
};

/**
 * Utility class to handle packet types and annotations
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
	static const uint8_t castor_paint_offset = castor_anno_offset + sizeof(Hash); // = 32, for documentation purposes only
};

CLICK_ENDDECLS
#endif
