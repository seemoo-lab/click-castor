#ifndef CLICK_CASTOR_H
#define CLICK_CASTOR_H

#include <click/ipaddress.hh>
#include <click/packet_anno.hh>

#define ETHERTYPE_CASTOR		0x0CA0
#define CASTOR_CONTENT_TYPE_IP	0x0800

#define CASTOR_HASHLENGTH		20
#define CASTOR_FLOWSIZE		   256	// Number of flow auth elements in the header
#define CASTOR_FLOWAUTH_ELEM	 8  // log2(CASTOR_FLOW_SIZE)#define CASTOR_MAX_GROUP_SIZE	10	// Maximal allowed group size

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

typedef uint8_t Hash[CASTOR_HASHLENGTH];

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
	IPAddress	src;
	IPAddress	dst;
	FlowId	 	fid;
	PacketId 	pid;
	FlowAuth 	fauth;
	EACKAuth 	eauth;
	uint8_t		hopcount; // Unprotected! For evaluation purposes only
} Castor_PKT;

// The ACK Header Structure
typedef struct {
	uint8_t 	type;
	uint8_t 	hsize;
	uint16_t 	len;
	ACKAuth 	auth;
} Castor_ACK;

/**
 * The Castor Class with utility functions to handle Packet Processing
 */
class CastorPacket{
public:

	static inline uint8_t getType(Packet* p){
		uint8_t type;
		memcpy(&type, p->data(), sizeof(type));
		type = type & 0xF0;
		return type;
	}

	static inline bool getCastorPKTHeader(Packet* p, Castor_PKT* header){
		if(getType(p) == CastorType::PKT){
			// Copy the header from packet
			memcpy(header, p->data(), sizeof(Castor_PKT));
			return true;
		}
		return false;
	}

	static inline bool getCastorACKHeader(Packet* p, Castor_ACK* header){
		if(getType(p) == CastorType::ACK){
			// Copy the header from packet
			memcpy(header, p->data(), sizeof(Castor_ACK));
			return true;
		}
		return false;
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
		uint8_t type;
		memcpy(&type, p->data(), sizeof(type));
		type = type & 0x0F;
		return (type == CastorType::XCAST);
	}

	static inline String hexToString(const unsigned char* hex, uint8_t length) {
		char buffer[2*length];
		for(int i=0; i<length; i++){
			snprintf(buffer + 2*i, 3,"%02x ", hex[i]);
		}
		return String(buffer);
	}

private:
	static const uint8_t src_ip_anno_offset = DST_IP_ANNO_OFFSET + DST_IP_ANNO_SIZE;
	static const uint8_t mac_ip_anno_offset = src_ip_anno_offset + DST_IP_ANNO_SIZE;
	static const uint8_t castor_anno_offset = mac_ip_anno_offset + DST_IP_ANNO_SIZE;

};

CLICK_ENDDECLS
#endif
